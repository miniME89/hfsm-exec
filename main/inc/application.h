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

#define LOGGER_APPLICATION "application"

#define APPLICATION_NAME "hfsm-exec"
#define APPLICATION_VERSION "0.5"
#define APPLICATION_DESCRIPTION "some description"

#include <logger.h>
#include <api.h>
#include <decoder.h>
#include <statemachine.h>
#include <plugins.h>

#include <QCoreApplication>
#include <QCommandLineParser>

namespace hfsmexec
{
    class Application
    {
        public:
            static Application* getInstance();

            Application(int argc, char** argv);
            ~Application();

            int exec();
            void quit();

            QCoreApplication* getQtApplication();
            DecoderProvider* getDecoderProvider();
            CommunicationPluginLoader* getCommunicationPluginLoader();

            bool getCommandLineOption(const QString& optionName, QStringList* values = NULL);

            bool postEvent(AbstractEvent* event);

            bool loadStateMachine(StateMachine* stateMachine);
            bool loadStateMachine(const QString& data);
            bool unloadStateMachine();

            bool startStateMachine();
            bool stopStateMachine();

        private:
            static Application* instance;
            static const Logger* logger;

            QCommandLineParser commandLineParser;
            QMap<QString, QCommandLineOption*> commandLineOptions;

            QCoreApplication qtApplication;
            DecoderProvider decoderProvider;
            CommunicationPluginLoader communicationPluginLoader;
            Api api;

            StateMachine* stateMachine;

            static void signalHandler(int signal);

            void createCommandLineOptions();
            void processCommandLineOptions();
    };
}

#endif
