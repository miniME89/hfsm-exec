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

#ifndef PLUGIN_ROS_H
#define PLUGIN_ROS_H

#include <plugins.h>

#include <QTcpSocket>

class RosCommunicationPlugin : public QObject, public hfsmexec::CommunicationPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hfsmexec.Plugins.CommunicationPlugin")
    Q_INTERFACES(hfsmexec::CommunicationPlugin)

    public:
        RosCommunicationPlugin();
        virtual ~RosCommunicationPlugin();

        virtual CommunicationPlugin* create();
        virtual bool invoke(hfsmexec::Value& endpoint, hfsmexec::Value& inputParameters, hfsmexec::Value& outputParameters);
        virtual bool cancel();

    public slots:
        void connected();
        void disconnected();
        void read();
        bool write(const hfsmexec::Value& value);
        bool write(const hfsmexec::Value& value, std::function<bool(hfsmexec::Value)> listener);

    private:
        QList<std::function<bool(hfsmexec::Value)> > listeners;
        hfsmexec::Value endpoint;
        hfsmexec::Value inputParameters;
        hfsmexec::Value outputParameters;
        QTcpSocket socket;

        bool publishMessage();
        bool subscribeMessage();
        bool sendServiceRequest();
        bool sendActionGoal();
};

#endif
