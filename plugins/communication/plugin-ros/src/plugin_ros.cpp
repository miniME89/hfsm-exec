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

RosCommunicationPlugin::RosCommunicationPlugin() :
    CommunicationPlugin("ROS")
{
    connect(&socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(&socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(&socket, SIGNAL(readyRead()), this, SLOT(read()));
}

RosCommunicationPlugin::~RosCommunicationPlugin()
{
    socket.close();
}

CommunicationPlugin* RosCommunicationPlugin::create()
{
    return new RosCommunicationPlugin();
}

bool RosCommunicationPlugin::invoke(Value& endpoint, Value& inputParameters, Value& outputParameters)
{
    this->endpoint = &endpoint;
    this->inputParameters = &inputParameters;
    this->outputParameters = &outputParameters;

    socket.connectToHost("localhost", 9090);

    return true;
}

bool RosCommunicationPlugin::cancel()
{

    return true;
}

void RosCommunicationPlugin::connected()
{
    logger->warning("connected");

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
}

void RosCommunicationPlugin::disconnected()
{
    logger->warning("disconnected");
}

void RosCommunicationPlugin::read()
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

    for (int i = listeners.size() - 1; i >= 0; i--)
    {
        if (!listeners[i](value))
        {
            listeners.removeAt(i);
        }
    }
}

bool RosCommunicationPlugin::write(const hfsmexec::Value& value)
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

bool RosCommunicationPlugin::write(const Value& value, std::function<bool(Value)> listener)
{
    if (!write(value))
    {
        return false;
    }

    listeners.append(listener);
}

bool RosCommunicationPlugin::publishMessage()
{
    logger->info("ROS communication: publish message");

    Value value;
    value = Value();
    value["op"] = "publish";
    value["topic"] = endpoint["topic"].getString();
    value["msg"] = inputParameters;

    return write(value);
}

bool RosCommunicationPlugin::subscribeMessage()
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

        write(value);

        outputParameters = message["msg"];

        return false;
    };

    return write(subscribe, listener);
}

bool RosCommunicationPlugin::sendServiceRequest()
{
    logger->info("ROS communication: service");

    Value request;
    request["op"] = "call_service";
    request["service"] = endpoint["topic"].getString();
    request["args"] = inputParameters;

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

        outputParameters = message["values"];
    };

    return write(request, listener);
}

bool RosCommunicationPlugin::sendActionGoal()
{
    logger->info("ROS communication: action");

    //subscribe to action result topic
    Value subscribe;
    subscribe["op"] = "subscribe";
    subscribe["topic"] = endpoint["topic"].getString() + "/result";
    subscribe["id"] = QUuid::createUuid().toString();

    if (!write(subscribe))
    {
        return false;
    }

    //send action goal
    Value publish;
    publish["op"] = "publish";
    publish["topic"] = endpoint["topic"].getString() + "/goal";
    publish["msg"]["goal_id"]["id"] = QUuid::createUuid().toString();
    publish["msg"]["goal"] = inputParameters;

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

        write(value);

        outputParameters = message["msg"];

        return false;
    };

    return write(publish, listener);
}
