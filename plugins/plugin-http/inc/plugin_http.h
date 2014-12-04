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

#ifndef PLUGIN_HTTP_H
#define PLUGIN_HTTP_H

#include <statemachine.h>
#include <plugins.h>

#include <QtPlugin>

namespace hfsmexec
{
    class HTTPCommunicationPlugin : public QObject, public CommunicationPlugin
    {
        Q_OBJECT
        Q_INTERFACES(hfsmexec::CommunicationPlugin)

        public:
            HTTPCommunicationPlugin();
            virtual ~HTTPCommunicationPlugin();

            virtual bool invoke(ValueContainer& endpoint, ValueContainer& inputParameters, ValueContainer& outputParameters);
            virtual bool cancel();
    };
}

#endif
