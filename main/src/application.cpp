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
#include <value_container.h>

#include <easylogging++.h>

_INITIALIZE_EASYLOGGINGPP

using namespace hfsmexec;

/*
 * Application
 */
Application* Application::application = NULL;

Application* Application::instance()
{
    return application;
}

void Application::signalHandler(int signal)
{
    CLOG(INFO, LOG_APPLICATION) <<"received signal";

    application->quit();
}

int Application::exec(int argc, char** argv)
{
    application = new Application(argc, argv);

    return application->exec();
}

Application::Application(int argc, char** argv) :
    logger(new Logger(argc, argv)),
    qtApplication(new QCoreApplication(argc, argv)),
    decoderProvider(new DecoderProvider()),
    communicationPluginLoader(new CommunicationPluginLoader())
{

}

Application::~Application()
{
    delete qtApplication;
    delete decoderProvider;
    delete communicationPluginLoader;
}

int Application::exec()
{
    CLOG(INFO, LOG_APPLICATION) <<"start application";

    //signal handler
    signal(SIGINT, Application::signalHandler);

    communicationPluginLoader->load("plugins");

    //test
    StateMachineTest stateMachineTest;
    //ValueContainerTest valueContainerTest;
    //DecoderTest decoderTest;
    //CommunicationPluginLoaderTest pluginTest;

    Api::exec();

    return qtApplication->exec();
}

int Application::quit()
{
    CLOG(INFO, LOG_APPLICATION) <<"stop application";

    Api::quit();
    Application::instance()->getQtApplication()->quit();

    return 0;
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

bool Application::postEvent(AbstractEvent* event)
{
    //post event to all state machines
    QList<StateMachine*> stateMachines = StateMachinePool::getInstance()->getPool();
    for (int i = 0; i < stateMachines.size(); i++)
    {
        stateMachines[i]->postEvent(event); //TODO copy event???
    }
}

bool Application::loadStateMachine(StateMachine* stateMachine)
{

}

bool Application::unloadStateMachine()
{

}

bool Application::stateMachineStart()
{

}

bool Application::stateMachineStop()
{

}
