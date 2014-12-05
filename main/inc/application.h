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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <api.h>
#include <logger.h>
#include <decoder_impl.h>
#include <statemachine_impl.h>
#include <plugins.h>

#include <QCoreApplication>


namespace hfsmexec
{
    class Application
    {
        public:
            static Application* instance();
            static void signalHandler(int signal);
            static int exec(int argc, char** argv);

            ~Application();

            QCoreApplication* getQtApplication();
            DecoderProvider* getDecoderProvider();
            CommunicationPluginLoader* getCommunicationPluginLoader();

            bool postEvent(AbstractEvent* event);

            bool loadStateMachine(StateMachine* stateMachine);
            bool unloadStateMachine();

            bool stateMachineStart();
            bool stateMachineStop();

        private:
            static Application* application;

            Logger* logger;
            QCoreApplication* qtApplication;
            DecoderProvider* decoderProvider;
            CommunicationPluginLoader* communicationPluginLoader;

            Application(int argc, char** argv);

            int exec();
            int quit();
    };
}

#endif
