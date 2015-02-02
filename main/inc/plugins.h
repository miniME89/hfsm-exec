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

namespace hfsmexec
{
    class CommunicationPlugin
    {
        public:
            CommunicationPlugin(const QString& pluginId);
            virtual ~CommunicationPlugin();

            const QString& getPluginId() const;

            virtual bool invoke(Parameter& endpoint, Parameter& inputParameters, Parameter& outputParameters) = 0;
            virtual bool cancel() = 0;

        protected:
            static const Logger* logger;
            const QString pluginId;
    };

    class DecoderPlugin
    {
        public:
            DecoderPlugin(const QString& pluginId, const QString& encoding);
            virtual ~DecoderPlugin();

            const QString& getPluginId() const;
            const QString& getEncoding() const;

            virtual StateMachine* decode(const QString& data) = 0;

        protected:
            static const Logger* logger;
            const QString pluginId;
            const QString encoding;
    };

    class PluginLoader
    {
        public:
            PluginLoader();
            ~PluginLoader();

            CommunicationPlugin* getCommunicationPlugin(const QString& pluginId);
            const QMap<QString, CommunicationPlugin*>& getCommunicationPlugins() const;

            DecoderPlugin* getDecoderPlugin(const QString& pluginId);
            const QMap<QString, DecoderPlugin*>& getDecoderPlugins() const;

            bool load(const QString& path);

        private:
            static const Logger* logger;
            QMap<QString, CommunicationPlugin*> communicationPlugins;
            QMap<QString, DecoderPlugin*> decoderPlugins;

            bool loadCommunicationPlugin(QObject* plugin);
            bool loadDecoderPlugin(QObject* plugin);
    };
}

Q_DECLARE_INTERFACE(hfsmexec::CommunicationPlugin, "hfsmexec.Plugins.CommunicationPlugin")
Q_DECLARE_INTERFACE(hfsmexec::DecoderPlugin, "hfsmexec.Plugins.DecoderPlugin")

#endif
