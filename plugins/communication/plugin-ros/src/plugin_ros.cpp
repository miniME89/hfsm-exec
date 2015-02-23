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
Rosbridge::Rosbridge()
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
    for (int i = listeners.size() - 1; i >= 0; i--)
    {
        if (!listeners[i](value))
        {
            listeners.removeAt(i);
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

bool Rosbridge::write(const Value& value, std::function<bool(Value)> listener)
{
    if (!write(value))
    {
        return false;
    }

    listenersMutex.lock();
    listeners.append(listener);
    listenersMutex.unlock();

    return true;
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

bool RosCommunicationPlugin::invoke(Value& endpoint, Value& input, Value& output)
{
    this->endpoint = &endpoint;
    this->input = &input;
    this->output = &output;

    QString type = endpoint["type"].getString();
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

    return true;
}

bool RosCommunicationPlugin::cancel()
{
    return true;
}

void RosCommunicationPlugin::publishMessage()
{
    logger->info("ROS communication: publish message");

    Value value;
    value = Value();
    value["op"] = "publish";
    value["topic"] = endpoint["topic"].getString();
    value["msg"] = input;

    if (rosbridge.write(value))
    {
        success();
    }
    else
    {
        error();
    }
}

void RosCommunicationPlugin::subscribeMessage()
{
    logger->info("ROS communication: subscribe message");

    Value subscribe;
    subscribe["op"] = "subscribe";
    subscribe["topic"] = endpoint["topic"].getString();
    subscribe["id"] = QUuid::createUuid().toString();

    //message listener
    auto listener = [=](Value message) {
        if (message["op"].getString() != "publish")
        {
            return true;
        }

        if (message["topic"] != subscribe["topic"])
        {
            return true;
        }

        logger->info("received subscription message");

        //unsubscribe from topic
        Value value;
        value["op"] = "unsubscribe";
        value["topic"] = subscribe["topic"];
        value["id"] = subscribe["id"];

        rosbridge.write(value);

        output = message["msg"];

        success();

        return false;
    };

    if (!rosbridge.write(subscribe, listener))
    {
        error();
    }
}

void RosCommunicationPlugin::sendServiceRequest()
{
    logger->info("ROS communication: service");

    Value request;
    request["op"] = "call_service";
    request["service"] = endpoint["topic"].getString();
    request["args"] = input;

    //message listener
    auto listener = [=](Value message) {
        if (message["op"].getString() != "service_response")
        {
            return true;
        }

        if (message["service"] != request["service"])
        {
            return true;
        }

        logger->info("received service response");

        output = message["values"];

        success();

        return false;
    };

    if (!rosbridge.write(request, listener))
    {
        error();
    }
}

void RosCommunicationPlugin::sendActionGoal()
{
    logger->info("ROS communication: action");

    //subscribe to action result topic
    Value subscribe;
    subscribe["op"] = "subscribe";
    subscribe["topic"] = endpoint["topic"].getString() + "/result";
    subscribe["id"] = QUuid::createUuid().toString();

    if (!rosbridge.write(subscribe))
    {
        error();
    }

    //send action goal
    Value publish;
    publish["op"] = "publish";
    publish["topic"] = endpoint["topic"].getString() + "/goal";
    publish["msg"]["goal"] = input["goal"];
    publish["msg"]["goal_id"]["id"] = QUuid::createUuid().toString();

    //message listener
    auto listener = [=](Value message) {
        if (message["op"].getString() != "publish")
        {
            return true;
        }

        if (message["topic"] != subscribe["topic"])
        {
            return true;
        }

        if (message["msg"]["status"]["goal_id"]["id"] != publish["msg"]["goal_id"]["id"])
        {
            return true;
        }

        logger->info("received action result");

        //unsubscribe from action result topic
        Value value;
        value["op"] = "unsubscribe";
        value["topic"] = subscribe["topic"];
        value["id"] = subscribe["id"];

        rosbridge.write(value);

        output = message["msg"];

        success();

        return false;
    };

    if (!rosbridge.write(publish, listener))
    {
        error();
    }
}
