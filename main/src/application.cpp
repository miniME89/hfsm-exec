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
#include <decoder_impl.h>
#include <statemachine_impl.h>
#include <utils.h>

#include <signal.h>

using namespace hfsmexec;

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

    createCommandLineOptions();

    signal(SIGINT, Application::signalHandler);
}

Application::~Application()
{

}

int Application::exec()
{
    logger->info("start application");

    decoderProvider.addDecoder(new XmlDecoder());

    processCommandLineOptions();

    if (!getCommandLineOption("api"))
    {
        api.exec();
    }

    return qtApplication.exec();
}

void Application::quit()
{
    logger->info("stop application");

    unloadStateMachine();

    api.quit();
    Application::getInstance()->getQtApplication()->quit();
}

QCoreApplication* Application::getQtApplication()
{
    return &qtApplication;
}

DecoderProvider* Application::getDecoderProvider()
{
    return &decoderProvider;
}

CommunicationPluginLoader* Application::getCommunicationPluginLoader()
{
    return &communicationPluginLoader;
}

bool Application::getCommandLineOption(const QString& optionName, QStringList* values)
{
    if (!commandLineOptions.contains(optionName))
    {
        return false;
    }

    if (!commandLineParser.isSet(optionName))
    {
        return false;
    }

    if (values != NULL)
    {
        *values = commandLineParser.values(*commandLineOptions[optionName]);
    }

    return true;
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

    StateMachine* stateMachine = decoderProvider.decode("XML", data);
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

void Application::createCommandLineOptions()
{
    QCoreApplication::setApplicationName(APPLICATION_NAME);
    QCoreApplication::setApplicationVersion(APPLICATION_VERSION);

    commandLineParser.setApplicationDescription(APPLICATION_DESCRIPTION);

    //add options
    commandLineParser.addHelpOption();
    commandLineParser.addVersionOption();
    commandLineOptions["logfile"] = new QCommandLineOption(QStringList() <<"f" <<"logfile", "Set the filename (including the path) for the log file.", "filename");
    commandLineOptions["logger"] = new QCommandLineOption(QStringList() <<"l" <<"logger", "Enable only the specified logger. Possible Loggers are: api, application, decoder, plugin, statemachine, builder, value", "logger");
    commandLineOptions["plugindir"] = new QCommandLineOption(QStringList() <<"p" <<"plugindir", "Set the path to the directory where the plugins will be loaded from.", "directory");
    commandLineOptions["smfile"] = new QCommandLineOption(QStringList() <<"s" <<"smfile", "Set the filename (including the path) for the state machine file which will be loaded, build and executed after startup.", "filename");
    commandLineOptions["api"] = new QCommandLineOption(QStringList() <<"a" <<"api", "Disable the API. This will avoid the startup of the HTTP server.");

    for (QMap<QString, QCommandLineOption*>::iterator i = commandLineOptions.begin(); i != commandLineOptions.end(); i++)
    {
        commandLineParser.addOption(*i.value());
    }

    commandLineParser.process(qtApplication);
}

void Application::processCommandLineOptions()
{
    //logfile
    QString logFile = "hfsm-exec.log";
    QStringList logFileValues;
    if (getCommandLineOption("logfile", &logFileValues))
    {
        if (logFileValues.size() > 0)
        {
            logFile = logFileValues.first();
        }
    }

    Logger::setFilename(logFile);

    //plugindir
    QStringList pluginDirValues;
    if (getCommandLineOption("plugindir", &pluginDirValues))
    {
        for (int i = 0; i < pluginDirValues.size(); i++)
        {
            communicationPluginLoader.load(pluginDirValues[i]);
        }
    }
    else
    {
        communicationPluginLoader.load("plugins");
    }

    //smfile
    QStringList smfileValues;
    if (getCommandLineOption("smfile", &smfileValues))
    {
        if (smfileValues.size() > 0)
        {
            QString path = smfileValues.first();
            Downloader downloader;

            downloader.download(path, true);
            if (!downloader.getError())
            {
                QString stateMachine = downloader.getData();
                if (loadStateMachine(stateMachine))
                {
                    startStateMachine();
                }
            }
        }
    }
}

/*
 * main
 */
int main(int argc, char** argv)
{
    Application* application = new Application(argc, argv);

    return application->exec();
}
