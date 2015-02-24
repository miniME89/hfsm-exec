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

#include "plugin_http.h"

using namespace hfsmexec;

HTTPCommunicationPlugin::HTTPCommunicationPlugin() :
    CommunicationPlugin("HTTP")
{

}

HTTPCommunicationPlugin::~HTTPCommunicationPlugin()
{

}

CommunicationPlugin* HTTPCommunicationPlugin::create()
{
	return new HTTPCommunicationPlugin();
}

void HTTPCommunicationPlugin::invoke()
{
    output["/some/test/value"].set(5);
    logger->info("test");
}

void HTTPCommunicationPlugin::cancel()
{

}
