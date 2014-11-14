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

#include <statemachine.h>
#include <QMap>

namespace hfsmexec
{
    class CommunicationPlugin
    {
        public:
            virtual ~CommunicationPlugin() {}
            virtual bool invoke() = 0;
            virtual bool cancel() = 0;

            const QString& getPluginId() const;

        protected:
            QString pluginId;
    };

    class CommunicationPluginLoader
    {
        public:
            CommunicationPluginLoader();

            bool load(QMap<QString, CommunicationPlugin*>& plugins);
            bool load(const QString& path, QMap<QString, CommunicationPlugin*>& plugins);
    };

    class CommunicationPluginLoaderTest
    {
        public:
            CommunicationPluginLoaderTest();
    };
}

Q_DECLARE_INTERFACE(hfsmexec::CommunicationPlugin, "CommunicationPlugin")

#endif
