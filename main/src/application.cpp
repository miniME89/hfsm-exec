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
#include <value_container.h>
#include <plugins.h>

using namespace hfsmexec;

ValueContainer parameterServer; //TODO remove

/*
 * Application
 */
Application* Application::application = NULL;

Application* Application::instance()
{
    return application;
}

Application::Application(int argc, char** argv) :
    qtApplication(argc, argv)
{
    application = this;
}

Application::~Application()
{

}

QCoreApplication* Application::getQtApplication()
{
    return &qtApplication;
}

ApiExecutor* Application::getApiExecutor()
{
    return &apiExecutor;
}

DecoderProvider* Application::getDecoderProvider()
{
    return &decoderProvider;
}

void Application::start()
{
    //StateMachineTest stateMachineTest;
    ValueContainerTest valueContainerTest;
    //DecoderTest decoderTest;
    //CommunicationPluginLoaderTest pluginTest;

    apiExecutor.start();
    qtApplication.exec();
}

void Application::stop()
{
    //TODO
}

bool Application::getParameter(const QString& path, QString& data)
{
    //return parameterServer.toJson(path, data);
}

bool Application::setParameter(const QString& path, const QString& data)
{
    //return parameterServer.fromJson(path, data);;
}

bool Application::deleteParameter(const QString& path)
{
    //parameterServer.remove(path);

    return true;
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
