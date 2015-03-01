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

#include <plugin_ros.h>
#include <QUuid>
#include <iostream>

using namespace hfsmexec;

const hfsmexec::Logger* Rosbridge::logger = hfsmexec::Logger::getLogger(LOGGER_PLUGIN);

/*
 * Rosbridge
 */
Rosbridge::Rosbridge() :
    id(0)
{
    connect(&socket, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(&socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
    connect(&socket, SIGNAL(readyRead()), this, SLOT(read()));

    socket.connectToHost("localhost", 9090);
}

Rosbridge::~Rosbridge()
{
    socket.close();
}

void Rosbridge::socketConnected()
{
    logger->info("connected");
}

void Rosbridge::socketError(QAbstractSocket::SocketError socketError)
{
    logger->warning(socket.errorString());
}

void Rosbridge::read()
{
    QByteArray data = socket.readLine();
    QString message(data);

    logger->info("read rosbridge message: " + message);

    Value value;
    if (!value.fromJson(message))
    {
        logger->warning("couldn't decode JSON data");

        return;
    }

    listenersMutex.lock();
    QMapIterator<int, std::function<bool(hfsmexec::Value)>> it(listeners);
    while (it.hasNext())
    {
        it.next();
        if (it.value()(value))
        {
            listeners.remove(it.key());
        }
    }
    listenersMutex.unlock();
}

bool Rosbridge::write(const hfsmexec::Value& value)
{
    QString data;
    if (!value.toJson(data))
    {
        logger->warning("couldn't encode JSON data");

        return false;
    }

    logger->info("write rosbridge message: " + data);

    int num = socket.write(data.toUtf8());

    if (num != data.size())
    {
        logger->warning(QString("couldn't write all bytes to socket (%1 of %2)").arg(num).arg(data.size()));

        return false;
    }

    return true;
}

int Rosbridge::registerListener(std::function<bool(Value)> listener)
{
    listenersMutex.lock();
    int handle = id++;
    listeners[handle] = listener;
    listenersMutex.unlock();

    return handle;
}

void Rosbridge::unregisterListener(int handle)
{
    listenersMutex.lock();
    listeners.remove(handle);
    listenersMutex.unlock();
}

/*
 * RosCommunicationPlugin
 */
Rosbridge RosCommunicationPlugin::rosbridge;

RosCommunicationPlugin::RosCommunicationPlugin() :
    CommunicationPlugin("ROS")
{

}

RosCommunicationPlugin::~RosCommunicationPlugin()
{

}

CommunicationPlugin* RosCommunicationPlugin::create()
{
    return new RosCommunicationPlugin();
}

void RosCommunicationPlugin::invoke()
{
    QString type = endpoint["type"].getString();
    logger->info(QString("ROS communication type is \"%1\"").arg(type));
    if (type == "publish")
    {
        publishMessage();
    }
    else if (type == "subscribe")
    {
        subscribeMessage();
    }
    else if (type == "service")
    {
        sendServiceRequest();
    }
    else if (type == "action")
    {
        sendActionGoal();
    }
    else
    {
        error(QString("unknown ROS communication type \"%1\"").arg(type));
    }
}

void RosCommunicationPlugin::cancel()
{
    if (cancelCallback)
    {
        cancelCallback();
    }
}

void RosCommunicationPlugin::publishMessage()
{
    //publish to topic
    Value publish;
    publish = Value();
    publish["op"] = "publish";
    publish["topic"] = endpoint["topic"].getString();
    publish["msg"] = input;
    if (!rosbridge.write(publish))
    {
        error();
        return;
    }

    success();
}

void RosCommunicationPlugin::subscribeMessage()
{
    //subscribe to topic
    Value subscribe;
    subscribe["op"] = "subscribe";
    subscribe["topic"] = endpoint["topic"].getString();
    subscribe["id"] = QUuid::createUuid().toString();
    if (!rosbridge.write(subscribe))
    {
        error();
        return;
    }

    //callback: unsubscribe from topic
    auto unsubscribeTopic = [=]() {
        Value unsubscribe;
        unsubscribe["op"] = "unsubscribe";
        unsubscribe["topic"] = subscribe["topic"];
        unsubscribe["id"] = subscribe["id"];
        rosbridge.write(unsubscribe);
    };

    //callback: received message
    auto receivedMessage = [=](Value& message) {
        logger->info("received message");

        success(message["msg"]);
    };

    //register rosbridge callback
    int handle = rosbridge.registerListener([=](Value message) {
        if (!(message["op"].getString() == "publish" &&
              message["topic"] == subscribe["topic"]))
        {
            return false;
        }

        receivedMessage(message);
        unsubscribeTopic();

        return true;
    });

    //register cancel callback
    cancelCallback = [=]() {
        unsubscribeTopic();
        rosbridge.unregisterListener(handle);
    };
}

void RosCommunicationPlugin::sendServiceRequest()
{
    //service request
    Value request;
    request["op"] = "call_service";
    request["service"] = endpoint["topic"].getString();
    request["args"] = input;
    if (!rosbridge.write(request))
    {
        error();
        return;
    }

    //callback: received response
    auto receivedResponse = [=](Value& message) {
        logger->info("received service response");

        success(message["values"]);
    };

    //register rosbridge callback
    int handle = rosbridge.registerListener([=](Value message) {
        if (!(message["op"].getString() == "service_response" &&
              message["service"] == request["service"]))
        {
            return false;
        }

        receivedResponse(message);

        return true;
    });

    //register cancel callback
    cancelCallback = [=]() {
        rosbridge.unregisterListener(handle);
    };
}

void RosCommunicationPlugin::sendActionGoal()
{
    //subscribe to action result topic
    Value subscribe;
    subscribe["op"] = "subscribe";
    subscribe["topic"] = endpoint["topic"].getString() + "/result";
    subscribe["id"] = QUuid::createUuid().toString();
    if (!rosbridge.write(subscribe))
    {
        error();
        return;
    }

    //send action goal
    Value publish;
    publish["op"] = "publish";
    publish["topic"] = endpoint["topic"].getString() + "/goal";
    publish["msg"]["goal"] = input["goal"];
    publish["msg"]["goal_id"]["id"] = QUuid::createUuid().toString();
    if (!rosbridge.write(publish))
    {
        error();
        return;
    }

    //callback: unsubscribe from result topic
    auto unsubscribeResultTopic = [=]() {
        Value unsubscribe;
        unsubscribe["op"] = "unsubscribe";
        unsubscribe["topic"] = subscribe["topic"];
        unsubscribe["id"] = subscribe["id"];
        rosbridge.write(unsubscribe);
    };

    //callback: cancel goal
    auto cancelGoal = [=]() {
        Value cancel;
        cancel["op"] = "publish";
        cancel["topic"] = endpoint["topic"].getString() + "/cancel";
        cancel["msg"]["id"] = publish["msg"]["goal_id"]["id"];
        rosbridge.write(cancel);
    };

    //callback: received result
    auto receivedResult = [=](Value& message) {
        logger->info("received action result");

        success(message["msg"]);
    };

    //register rosbridge callback
    int handle = rosbridge.registerListener([=](Value message) {
        if (!(message["op"].getString() == "publish" &&
              message["topic"] == subscribe["topic"] &&
              message["msg"]["status"]["goal_id"]["id"] == publish["msg"]["goal_id"]["id"]))
        {
            return false;
        }

        receivedResult(message);
        unsubscribeResultTopic();

        return true;
    });

    //register cancel callback
    cancelCallback = [=]() {
        cancelGoal();
        unsubscribeResultTopic();
        rosbridge.unregisterListener(handle);
    };
}
