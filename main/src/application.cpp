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
#include <logger.h>

#include <easylogging++.h>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>

_INITIALIZE_EASYLOGGINGPP

using namespace hfsmexec;

/*
 * Application
 */
Application* Application::instance = NULL;

Application* Application::getInstance()
{
    return instance;
}

void Application::signalHandler(int signal)
{
    CLOG(INFO, LOG_APPLICATION) <<"received signal";

    instance->quit();
}

Application::Application(int argc, char** argv) :
    logger(new Logger()),
    qtApplication(new QCoreApplication(argc, argv)),
    decoderProvider(new DecoderProvider()),
    communicationPluginLoader(new CommunicationPluginLoader()),
    apiWorker(new ApiWorker())
{
    instance = this;

    signal(SIGINT, Application::signalHandler);

    decoderProvider->addDecoder(new XmlDecoder());

    createCommandLineOptions();
    processCommandLineOptions();
}

Application::~Application()
{
    delete logger;
    delete qtApplication;
    delete decoderProvider;
    delete communicationPluginLoader;
}

int Application::exec()
{
    CLOG(INFO, LOG_APPLICATION) <<"start application";

    if (!getCommandLineOption("api"))
    {
        apiWorker->exec();
    }

    return qtApplication->exec();
}

void Application::quit()
{
    CLOG(INFO, LOG_APPLICATION) <<"stop application";

    unloadStateMachine();

    apiWorker->quit();
    Application::getInstance()->getQtApplication()->quit();
}

Logger* Application::getLogger()
{
    return logger;
}

QCoreApplication* Application::getQtApplication()
{
    return qtApplication;
}

DecoderProvider* Application::getDecoderProvider()
{
    return decoderProvider;
}

CommunicationPluginLoader* Application::getCommunicationPluginLoader()
{
    return communicationPluginLoader;
}

ApiWorker* Application::getApiWorker()
{
    return apiWorker;
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
    CLOG(INFO, LOG_APPLICATION) <<"post event to the executing state machine";

    if (stateMachine != NULL)
    {
        stateMachine->postEvent(event);

        return true;
    }

    return false;
}

bool Application::loadStateMachine(StateMachine* stateMachine)
{
    CLOG(INFO, LOG_APPLICATION) <<"load state machine";

    if (!unloadStateMachine())
    {
        CLOG(WARNING, LOG_APPLICATION) <<"couldn't load state machine: unloading of existing state machine failed";

        return false;
    }

    this->stateMachine = stateMachine;

    return true;
}

bool Application::loadStateMachine(const QString& data)
{
    CLOG(INFO, LOG_APPLICATION) <<"load state machine from encoded data";

    StateMachine* stateMachine = decoderProvider->decode("XML", data);
    if (stateMachine == NULL)
    {
        CLOG(WARNING, LOG_APPLICATION) <<"couldn't load state machine: decoding of encoded state machine failed";

        return false;
    }

    loadStateMachine(stateMachine);

    return true;
}

bool Application::loadStateMachine(const QUrl& url)
{
    CLOG(INFO, LOG_APPLICATION) <<"load state machine from " <<url.toString();

    QNetworkAccessManager manager;
    QNetworkReply* reply = manager.get(QNetworkRequest(url));

    while (!reply->isFinished())
    {
        qtApplication->processEvents();
    }

    QString stateMachine(reply->readAll());
    CLOG(INFO, LOG_APPLICATION) <<stateMachine;

    return loadStateMachine(stateMachine);
}

bool Application::unloadStateMachine()
{
    CLOG(INFO, LOG_APPLICATION) <<"unload the currently loaded state machine";

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
    CLOG(INFO, LOG_APPLICATION) <<"start the loaded state machine";

    if (stateMachine == NULL)
    {
        CLOG(WARNING, LOG_APPLICATION) <<"couldn't start the loaded state machine: no state machine was loaded";

        return false;
    }

    stateMachine->start();

    return true;
}

bool Application::stopStateMachine()
{
    CLOG(INFO, LOG_APPLICATION) <<"stop the executing state machine";

    if (stateMachine == NULL)
    {
        CLOG(WARNING, LOG_APPLICATION) <<"couldn't stop the loaded state machine: no state machine was loaded";

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
}

void Application::processCommandLineOptions()
{
    commandLineParser.process(*qtApplication);

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

    logger->setFilename(logFile);

    //logger
    QStringList loggersValues;
    if (getCommandLineOption("logger", &loggersValues))
    {
        if (loggersValues.size() > 0)
        {
            logger->setLoggerEnabled(false);

            for (int i = 0; i < loggersValues.size(); i++)
            {
                logger->setLoggerEnabled(loggersValues[i], true);
            }
        }
    }

    //plugindir
    QStringList pluginDirValues;
    if (getCommandLineOption("plugindir", &pluginDirValues))
    {
        for (int i = 0; i < pluginDirValues.size(); i++)
        {
            communicationPluginLoader->load(pluginDirValues[i]);
        }
    }
    else
    {
        communicationPluginLoader->load("plugins");
    }

    //smfile
    QStringList smfileValues;
    if (getCommandLineOption("smfile", &smfileValues))
    {
        if (smfileValues.size() > 0)
        {
            QUrl url;
            QString path = smfileValues.first();
            //remote file
            if (path.contains("://"))
            {
                url = QUrl::fromUserInput(path);
            }
            //local file
            else
            {
                url = QUrl::fromLocalFile(path);
            }

            if (loadStateMachine(url))
            {
                startStateMachine();
            }
        }
    }
}
