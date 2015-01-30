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

#ifndef API_H
#define API_H

#define LOGGER_API "api"

#include <logger.h>
#include <httpserver.h>

#include <jsoncpp/json/json.h>

#include <QRegExp>

namespace hfsmexec
{
    class Api
    {
        public:
            Api();
            ~Api();

            void exec();
            void quit();

        private:
            typedef struct Service
            {
                QString pattern;
                QString method;
                QRegExp regex;
                std::function<void(HttpRequest*, HttpResponse*)> handler;
            } Service;

            static const Logger* logger;
            HttpServer server;
            QList<Service> services;

            PushNotification logPushNotification;

            void logListener(const QString& name, Logger::Level level, const QString& message);

            void log(HttpRequest* request, HttpResponse* response);
            void statemachineLoad(HttpRequest* request, HttpResponse* response);
            void statemachineUnload(HttpRequest* request, HttpResponse* response);
            void statemachineStart(HttpRequest* request, HttpResponse* response);
            void statemachineStop(HttpRequest* request, HttpResponse* response);
            void statemachineEvent(HttpRequest* request, HttpResponse* response);

            void assign(QString pattern, QString method, std::function<void(HttpRequest*, HttpResponse*)> handler);
            void httpHandler(HttpRequest* request, HttpResponse* response);
    };
}

#endif
