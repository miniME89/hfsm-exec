/*
 *  Copyright (C) 2014 Marcel Lehwald
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <application.h>

#include <signal.h>

#include <QCommandLineParser>
#include <QFile>
#include <QTextStream>

using namespace hfsmexec;

/*
 * Configuration
 */
Configuration::Configuration()
{
    api = false;
    apiPort = 8080;
    loggerFile = "hfsm-exec.log";
    pluginDirs = QStringList() <<"plugins";
}

Configuration::~Configuration()
{

}

void Configuration::load()
{
    QCommandLineParser commandLineParser;

    QCoreApplication::setApplicationName(APPLICATION_NAME);
    QCoreApplication::setApplicationVersion(APPLICATION_VERSION);

    commandLineParser.setApplicationDescription(APPLICATION_DESCRIPTION);

    //add options
    QCommandLineOption commandLogger(QStringList() <<"l" <<"logger", "Enable only the specified loggers. Possible Loggers are: api, application, builder, parameter, plugin, statemachine. [Default: all]", "logger");
    QCommandLineOption commandLoggerFile(QStringList() <<"f" <<"logger-file", "Set the filename (including the path) for the log file.", "filename");
    QCommandLineOption commandPluginDir(QStringList() <<"d" <<"plugin-dir", "Set the path to the directories where the plugins will be loaded from. [Default: ./plugins/]", "directory");
    QCommandLineOption commandApi(QStringList() <<"a" <<"api", "Enable the REST API. This will startup the internal HTTP server.");
    QCommandLineOption commandApiPort(QStringList() <<"p" <<"api-port", "Set port of the HTTP server for the REST API. [Default: 8080]", "port");
    QCommandLineOption commandImportStatemachine(QStringList() <<"i" <<"import", "Import a state machine.", "filename");
    QCommandLineOption commandExportStatemachine(QStringList() <<"o" <<"export", "Export the imported state machine.", "filename");
    QCommandLineOption commandEncoding(QStringList() <<"e" <<"encoding", "Encoding of the imported/exported state machine.", "encoding");

    commandLineParser.addHelpOption();
    commandLineParser.addVersionOption();
    commandLineParser.addOption(commandLogger);
    commandLineParser.addOption(commandLoggerFile);
    commandLineParser.addOption(commandPluginDir);
    commandLineParser.addOption(commandApi);
    commandLineParser.addOption(commandApiPort);
    commandLineParser.addOption(commandImportStatemachine);
    commandLineParser.addOption(commandExportStatemachine);
    commandLineParser.addOption(commandEncoding);

    //process command line
    commandLineParser.process(Application::getInstance()->getQtApplication());

    //logger
    if (commandLineParser.isSet(commandLogger))
    {
        loggers = commandLineParser.values(commandLogger);
    }

    //logger file
    if (commandLineParser.isSet(commandLoggerFile))
    {
        loggerFile = commandLineParser.value(commandLoggerFile);
    }

    //plugin dir
    if (commandLineParser.isSet(commandPluginDir))
    {
        pluginDirs = commandLineParser.values(commandPluginDir);
    }

    //api
    if (commandLineParser.isSet(commandApi))
    {
        api = true;
    }

    //api port
    if (commandLineParser.isSet(commandApiPort))
    {
        apiPort = commandLineParser.value(commandApiPort).toInt();
    }

    //import
    if (commandLineParser.isSet(commandImportStatemachine))
    {
        importStateMachine = commandLineParser.value(commandImportStatemachine);
    }

    //export
    if (commandLineParser.isSet(commandExportStatemachine))
    {
        exportStateMachine = commandLineParser.value(commandExportStatemachine);
    }

    //encoding
    if (commandLineParser.isSet(commandEncoding))
    {
        QStringList encodings = commandLineParser.values(commandEncoding);
        if (encodings.size() == 1)
        {
            importEncoding = encodings[0];
        }
        else if (encodings.size() == 2)
        {
            importEncoding = encodings[0];
            exportEncoding = encodings[1];
        }
    }
}

/*
 * Application
 */
Application* Application::instance = NULL;

const Logger* Application::logger = Logger::getLogger(LOGGER_APPLICATION);

Application* Application::getInstance()
{
    return instance;
}

void Application::signalHandler(int signal)
{
    logger->info(QString("received signal (%1)").arg(signal));

    instance->quit();
}

Application::Application(int argc, char** argv) :
    qtApplication(argc, argv),
    stateMachine(NULL)
{
    instance = this;

    setlocale(LC_NUMERIC, "C");

    configuration.load();
}

Application::~Application()
{

}

int Application::exec()
{
    logger->info("start application");

    signal(SIGINT, Application::signalHandler);

    //enable loggers
    if (configuration.loggers.size() > 0)
    {
        Logger::setLoggerEnabled(false);
    }

    for (int i = 0; i < configuration.loggers.size(); i++)
    {
        Logger::setLoggerEnabled(configuration.loggers[i], true);
    }

    //load plugins
    for (int i = 0; i < configuration.pluginDirs.size(); i++)
    {
        pluginLoader.load(configuration.pluginDirs[i]);
    }

    //enable API
    if (configuration.api)
    {
        api.exec(configuration.apiPort);
    }

    //import state machine
    if (!configuration.importStateMachine.isEmpty())
    {
        QFile file(configuration.importStateMachine);
        if (file.open(QIODevice::ReadOnly))
        {
            QTextStream stream(&file);
            QString data = stream.readAll();
            file.close();

            loadStateMachine(configuration.importEncoding, data);
        }
    }

    //export state machine
    if (!configuration.exportStateMachine.isEmpty() && stateMachine != NULL)
    {
        QFile file(configuration.exportStateMachine);
        if (file.open(QIODevice::WriteOnly))
        {
            ExporterPlugin* exporter = pluginLoader.getExporterPlugin(configuration.exportEncoding);
            if (exporter != NULL)
            {
                QString data = exporter->exportStateMachine(stateMachine);
                QTextStream stream(&file);
                stream <<data;
                file.close();
            }
        }
    }

    return qtApplication.exec();
}

void Application::quit()
{
    logger->info("stop application");

    unloadStateMachine();

    api.quit();
    qtApplication.quit();
}

Configuration& Application::getConfiguration()
{
    return configuration;
}

QCoreApplication& Application::getQtApplication()
{
    return qtApplication;
}

PluginLoader& Application::getCommunicationPluginLoader()
{
    return pluginLoader;
}

Api& Application::getApi()
{
    return api;
}

bool Application::postEvent(AbstractEvent* event)
{
    logger->info("post event to the executing state machine");

    if (stateMachine != NULL)
    {
        stateMachine->postEvent(event);

        return true;
    }

    return false;
}

bool Application::loadStateMachine(const QString& encoding, const QString& data)
{
    if (!unloadStateMachine())
    {
        logger->warning("couldn't load state machine: unloading of existing state machine failed");

        return false;
    }

    logger->info(QString("load state machine with \"%1\" encoding").arg(encoding));

    ImporterPlugin* importerPlugin = pluginLoader.getImporterPlugin(encoding);
    if (importerPlugin == NULL)
    {
        logger->warning(QString("couldn't load state machine: no suitable importer plugin loaded for \"%1\" encoding").arg(encoding));

        return false;
    }

    StateMachine* stateMachine = importerPlugin->importStateMachine(data);
    if (stateMachine == NULL)
    {
        logger->warning(QString("couldn't load state machine: importing of state machine failed").arg(encoding));

        return false;
    }

    logger->info("loaded state machine");

    this->stateMachine = stateMachine;

    return true;
}

bool Application::unloadStateMachine()
{
    if (stateMachine == NULL)
    {
        return true;
    }

    if (!stopStateMachine())
    {
        return false;
    }

    logger->info("unload the currently loaded state machine");

    delete stateMachine;
    stateMachine = NULL;

    return true;
}

bool Application::startStateMachine()
{
    if (stateMachine == NULL)
    {
        logger->warning("couldn't start the loaded state machine: no state machine was loaded");

        return false;
    }

    logger->info("start the loaded state machine");

    stateMachine->start();

    return true;
}

bool Application::stopStateMachine()
{
    if (stateMachine == NULL)
    {
        return true;
    }

    logger->info("stop the executing state machine");

    stateMachine->stop();

    return true;
}

/*
 * main
 */
int main(int argc, char** argv)
{
    Application* application = new Application(argc, argv);

    return application->exec();
}
