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
    logFile = "hfsm-exec.log";
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
    QCommandLineOption commandLogger(QStringList() <<"l" <<"logger", "Enable only the specified loggers. Possible Loggers are: api, application, builder, decoder, parameter, plugin, statemachine. [Default: all]", "logger");
    QCommandLineOption commandLogFle(QStringList() <<"f" <<"logfile", "Set the filename (including the path) for the log file.", "filename");
    QCommandLineOption commandPluginDir(QStringList() <<"d" <<"plugindir", "Set the path to the directories where the plugins will be loaded from. [Default: ./plugins/]", "directory");
    QCommandLineOption commandApi(QStringList() <<"a" <<"api", "Enable the REST API. This will startup the internal HTTP server.");
    QCommandLineOption commandApiPort(QStringList() <<"p" <<"apiport", "Set port of the HTTP server for the REST API. [Default: 8080]", "port");
    QCommandLineOption commandStatemachine(QStringList() <<"s" <<"statemachine", "Set the filename (including the path) for the state machine file which will be loaded, build and executed after startup.", "filename");

    commandLineParser.addHelpOption();
    commandLineParser.addVersionOption();
    commandLineParser.addOption(commandLogger);
    commandLineParser.addOption(commandLogFle);
    commandLineParser.addOption(commandPluginDir);
    commandLineParser.addOption(commandApi);
    commandLineParser.addOption(commandApiPort);
    commandLineParser.addOption(commandStatemachine);

    //process command line
    commandLineParser.process(Application::getInstance()->getQtApplication());

    //logger
    if (commandLineParser.isSet(commandLogger))
    {
        loggers = commandLineParser.values(commandLogger);
    }

    //logfile
    if (commandLineParser.isSet(commandLogFle))
    {
        logFile = commandLineParser.value(commandLogFle);
    }

    //plugindir
    if (commandLineParser.isSet(commandPluginDir))
    {
        pluginDirs = commandLineParser.values(commandPluginDir);
    }

    //api
    if (commandLineParser.isSet(commandApi))
    {
        api = true;
    }

    //apiport
    if (commandLineParser.isSet(commandApiPort))
    {
        apiPort = commandLineParser.value(commandApiPort).toInt();
    }

    //statemachine
    if (commandLineParser.isSet(commandStatemachine))
    {
        statemachine = commandLineParser.value(commandStatemachine);
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

    //load statemachine from file
    if (!configuration.statemachine.isEmpty())
    {
        QFile file(configuration.statemachine);
        if (file.open(QIODevice::ReadOnly))
        {
            QTextStream stream(&file);
            QString stateMachine = stream.readAll();
            file.close();

            if (loadStateMachine(stateMachine))
            {
                startStateMachine();
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

bool Application::loadStateMachine(StateMachine* stateMachine)
{
    logger->info("load state machine");

    if (!unloadStateMachine())
    {
        logger->warning("couldn't load state machine: unloading of existing state machine failed");

        return false;
    }

    this->stateMachine = stateMachine;

    return true;
}

bool Application::loadStateMachine(const QString& data)
{
    logger->info("load state machine from encoded data");

    StateMachine* stateMachine = pluginLoader.getDecoderPlugin("FHG")->decode(data); //TODO
    if (stateMachine == NULL)
    {
        logger->warning("couldn't load state machine: decoding of encoded state machine failed");

        return false;
    }

    loadStateMachine(stateMachine);

    return true;
}

bool Application::unloadStateMachine()
{
    logger->info("unload the currently loaded state machine");

    if (stateMachine == NULL)
    {
        return true;
    }

    if (!stopStateMachine())
    {
        return false;
    }

    delete stateMachine;
    stateMachine = NULL;

    return true;
}

bool Application::startStateMachine()
{
    logger->info("start the loaded state machine");

    if (stateMachine == NULL)
    {
        logger->warning("couldn't start the loaded state machine: no state machine was loaded");

        return false;
    }

    stateMachine->start();

    return true;
}

bool Application::stopStateMachine()
{
    logger->info("stop the executing state machine");

    if (stateMachine == NULL)
    {
        logger->warning("couldn't stop the loaded state machine: no state machine was loaded");

        return false;
    }

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
