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

#include <api.h>
#include <application.h>
#include <statemachine_impl.h>

#include <jsoncpp/json/json.h>

using namespace hfsmexec;

/*
 * Api
 */
const Logger* Api::logger = Logger::getLogger(LOGGER_API);

Api::Api()
{
    server.setHandler(std::bind(&Api::httpHandler, this, std::placeholders::_1, std::placeholders::_2));

    assign("/log", "GET", std::bind(&Api::log, this, std::placeholders::_1, std::placeholders::_2));
    assign("/statemachine/load", "POST", std::bind(&Api::statemachineLoad, this, std::placeholders::_1, std::placeholders::_2));
    assign("/statemachine/unload", "POST", std::bind(&Api::statemachineUnload, this, std::placeholders::_1, std::placeholders::_2));
    assign("/statemachine/start", "POST", std::bind(&Api::statemachineStart, this, std::placeholders::_1, std::placeholders::_2));
    assign("/statemachine/stop", "POST", std::bind(&Api::statemachineStop, this, std::placeholders::_1, std::placeholders::_2));
    assign("/statemachine/event", "POST", std::bind(&Api::statemachineEvent, this, std::placeholders::_1, std::placeholders::_2));

    Logger::registerListener("api", std::bind(&Api::logListener, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

Api::~Api()
{

}

void Api::exec()
{
    logger->info("start HTTP server");

    if (!server.start())
    {
        logger->warning("couldn't start HTTP server");
    }
}

void Api::quit()
{
    logger->info("stop HTTP server");

    server.stop();
}

void Api::logListener(const QString& name, Logger::Level level, const QString& message)
{
    logPushNotification.write(message.toStdString());
}

void Api::log(HttpRequest* request, HttpResponse* response)
{
    int index = std::strtol(request->getHeader("Push-Notification-Index").c_str(), NULL, 10);
    std::string data;
    if (logPushNotification.read(index, data))
    {
        response->setStatusCode(200);
        response->setHeader("Push-Notification-Index", std::to_string(index + 1));
        response->write(data);
    }
    else
    {
        response->setStatusCode(304);
        response->setHeader("Push-Notification-Index", std::to_string(index));
    }
}

void Api::statemachineLoad(HttpRequest* request, HttpResponse* response)
{
    response->setStatusCode(200);
}

void Api::statemachineUnload(HttpRequest* request, HttpResponse* response)
{
    response->setStatusCode(200);
}

void Api::statemachineStart(HttpRequest* request, HttpResponse* response)
{
    response->setStatusCode(200);
}

void Api::statemachineStop(HttpRequest* request, HttpResponse* response)
{
    response->setStatusCode(200);
}

void Api::statemachineEvent(HttpRequest* request, HttpResponse* response)
{

}

void Api::assign(QString pattern, QString method, std::function<void(HttpRequest*, HttpResponse*)> handler)
{
    Service service;
    service.pattern = pattern;
    service.method = method;
    service.regex = QRegExp(pattern);
    service.handler = handler;

    services.append(service);
}

void Api::httpHandler(HttpRequest* request, HttpResponse* response)
{
    //allow CORS
    response->setHeader("Access-Control-Allow-Origin", "*");
    response->setHeader("Access-Control-Expose-Headers", "Push-Notification-Index");
    response->setHeader("Access-Control-Allow-Methods", "GET, POST, DELETE, PUT");
    response->setHeader("Access-Control-Allow-Headers", "Push-Notification-Index");

    //handle OPTIONS method send by browsers before actual request
    if (request->getMethod() == "OPTIONS")
    {
        response->setStatusCode(HttpResponse::STATUS_OK);

        return;
    }

    //lookup service
    for (int i = 0; i < services.size(); i++)
    {
        Service service = services[i];
        if (service.regex.exactMatch(request->getUrl().c_str()))
        {
            if (service.method == request->getMethod().c_str())
            {
                service.handler(request, response);

                return;
            }
        }
    }
}
