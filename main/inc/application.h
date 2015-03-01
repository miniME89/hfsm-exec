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
#define APPLICATION_DESCRIPTION ""

#include <logger.h>
#include <api.h>
#include <statemachine.h>
#include <plugins.h>

#include <QCoreApplication>
#include <QStringList>

namespace hfsmexec
{
    class Configuration
    {
        public:
            Configuration();
            ~Configuration();

            bool api;
            int apiPort;
            QString loggerFile;
            QStringList loggers;
            QStringList pluginDirs;
            QString importStateMachine;
            QString exportStateMachine;
            QString importEncoding;
            QString exportEncoding;

            void load();
    };

    class Application : public QObject
    {
        Q_OBJECT

        public:
            static Application* getInstance();

            Application(int argc, char** argv);
            ~Application();

            int exec();
            void quit();

            Configuration& getConfiguration();
            QCoreApplication& getQtApplication();
            PluginLoader& getCommunicationPluginLoader();
            Api& getApi();

        public slots:
            bool postEvent(AbstractEvent* event);

            bool loadStateMachine(const QString& encoding, const QString& data);
            bool unloadStateMachine();

            bool startStateMachine();
            bool stopStateMachine();

        private:
            static Application* instance;
            static const Logger* logger;

            Configuration configuration;
            QCoreApplication qtApplication;
            PluginLoader pluginLoader;
            Api api;

            StateMachine* stateMachine;

            static void signalHandler(int signal);
    };
}

#endif
