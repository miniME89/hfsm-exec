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

class Rosbridge : public QObject {
    Q_OBJECT

  public:
    Rosbridge();
    ~Rosbridge();

  public slots:
    void socketConnected();
    void socketError(QAbstractSocket::SocketError socketError);

    void read();
    bool write(const hfsmexec::Value& value);

    int registerListener(std::function<bool(hfsmexec::Value)> listener);
    void unregisterListener(int handle);

  private:
    static const hfsmexec::Logger* logger;
    QTcpSocket socket;
    QMap<int, std::function<bool(hfsmexec::Value)>> listeners;
    QMutex listenersMutex;
    int id;
};

class RosCommunicationPlugin : public QObject, public hfsmexec::CommunicationPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hfsmexec.Plugins.CommunicationPlugin")
    Q_INTERFACES(hfsmexec::CommunicationPlugin)

  public:
    RosCommunicationPlugin();
    virtual ~RosCommunicationPlugin();

    virtual CommunicationPlugin* create();
    virtual void invoke();
    virtual void cancel();

  private:
    static Rosbridge rosbridge;

    std::function<void()> cancelCallback;

    void publishMessage();
    void subscribeMessage();
    void sendServiceRequest();
    void sendActionGoal();
};

#endif
