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

#ifndef PLUGIN_LOADER_H
#define PLUGIN_LOADER_H

#include <statemachine.h>
#include <QMap>

namespace hfsmexec
{
    class InvokeStatePlugin
    {
        public:
            virtual ~InvokeStatePlugin() {}
            virtual bool invoke() = 0;
            virtual bool cancel() = 0;

            const QString& getPluginId() const;

        protected:
            QString pluginId;
    };

    class InvokeStatePluginLoader
    {
        public:
            InvokeStatePluginLoader();

            bool load(QMap<QString, InvokeStatePlugin*>& plugins);
            bool load(const QString& path, QMap<QString, InvokeStatePlugin*>& plugins);
    };

    class InvokeStatePluginLoaderTest
    {
        public:
            InvokeStatePluginLoaderTest();
    };
}

Q_DECLARE_INTERFACE(hfsmexec::InvokeStatePlugin, "InvokeStatePlugin")

#endif
