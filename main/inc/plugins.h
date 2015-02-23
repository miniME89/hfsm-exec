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

#ifndef PLUGINS_H
#define PLUGINS_H

#define LOGGER_PLUGIN "plugins"

#include <logger.h>
#include <statemachine.h>

#include <QtPlugin>
#include <QPluginLoader>

namespace hfsmexec
{
    class CommunicationPlugin
    {
        friend class InvokeState;

        public:
            CommunicationPlugin(const QString& pluginId);
            virtual ~CommunicationPlugin();

            const QString& getPluginId() const;

            void success();
            void error(QString message = "");

            virtual CommunicationPlugin* create() = 0;
            virtual bool invoke(Value& endpoint, Value& input, Value& output) = 0;
            virtual bool cancel() = 0;

        protected:
            static const Logger* logger;
            const QString pluginId;
            std::function<void()> successCallback;
            std::function<void(QString)> errorCallback;
    };

    class ImporterPlugin
    {
        public:
            ImporterPlugin(const QString& pluginId);
            virtual ~ImporterPlugin();

            const QString& getPluginId() const;

            virtual StateMachine* importStateMachine(const QString& data) = 0;

        protected:
            static const Logger* logger;
            const QString pluginId;
    };

    class ExporterPlugin
    {
        public:
            ExporterPlugin(const QString& pluginId);
            virtual ~ExporterPlugin();

            const QString& getPluginId() const;

            virtual QString exportStateMachine(StateMachine* stateMachine) = 0;

        protected:
            static const Logger* logger;
            const QString pluginId;
    };

    class PluginLoader
    {
        public:
            PluginLoader();
            ~PluginLoader();

            CommunicationPlugin* getCommunicationPlugin(const QString& pluginId);
            ImporterPlugin* getImporterPlugin(const QString& pluginId);
            ExporterPlugin* getExporterPlugin(const QString& pluginId);

            bool load(const QString& path);

        private:
            static const Logger* logger;
            QMap<QString, CommunicationPlugin*> communicationPlugins;
            QMap<QString, ImporterPlugin*> importerPlugins;
            QMap<QString, ExporterPlugin*> exporterPlugins;
    };
}

Q_DECLARE_INTERFACE(hfsmexec::CommunicationPlugin, "hfsmexec.Plugins.CommunicationPlugin")
Q_DECLARE_INTERFACE(hfsmexec::ImporterPlugin, "hfsmexec.Plugins.ImporterPlugin")
Q_DECLARE_INTERFACE(hfsmexec::ExporterPlugin, "hfsmexec.Plugins.ExporterPlugin")

#endif
