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

#include <QDebug>

#include <cppcms/http_response.h>
#include <cppcms/http_request.h>
#include <cppcms/http_context.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>

using namespace hfsmexec;

/*
 * Api
 */
Api::Api(cppcms::service &srv) :
    cppcms::application(srv),
    application(Application::instance())
{
    dispatcher().assign("/parameters/(.*)", &Api::handlerParameters, this, 1);
    mapper().assign("parameters", "/parameters/{1}");

    dispatcher().assign("/statemachine/event", &Api::handlerEvent, this);
    mapper().assign("events", "/statemachine/event");
}

Api::~Api()
{

}

void Api::main(std::string url)
{
    qDebug() <<QString(request().request_method().c_str()) <<QString(url.c_str());
    cppcms::application::main(url);
}

std::string Api::content()
{
    std::pair<void*, size_t> body = request().raw_post_data();

    return std::string((const char *)body.first, body.second);
}

void Api::handlerParameters(std::string path)
{
    if (request().request_method() == "GET")
    {
        QString data;
        if (application->getParameter(path.c_str(), data))
        {
            response().out() <<data.toStdString();
        }
    }
    else if (request().request_method() == "PUT")
    {
        application->setParameter(path.c_str(), content().c_str());
    }
    else if (request().request_method() == "DELETE")
    {
        application->deleteParameter(path.c_str());
    }
    else
    {
        response().make_error_response(403);
    }
}

void Api::handlerEvent()
{
    if (request().request_method() == "PUT")
    {
        //parse JSON
        cppcms::json::value event;
        std::istringstream stream(content());
        if (!event.load(stream, true))
        {
            response().out() <<"invalid JSON";

            return;
        }

        //validate structure
        if (event.type() != cppcms::json::is_object)
        {
            response().out() <<"invalid event structure";

            return;
        }

        if (event["name"].type() != cppcms::json::is_string)
        {
            response().out() <<"invalid event structure";

            return;
        }

        NamedEvent* namedEvent = new NamedEvent(event["name"].str().c_str());
        application->postEvent(namedEvent);
    }
    else
    {
        response().make_error_response(403);
    }
}

/*
 * ApiExecutor
 */
ApiExecutor::ApiExecutor()
{

}

ApiExecutor::~ApiExecutor()
{

}

void ApiExecutor::run()
{
    try
    {
        cppcms::json::value config;
        config["service"]["api"] = "http";
        config["service"]["port"] = 8080;

        cppcms::service service(config);
        service.applications_pool().mount(cppcms::applications_factory<Api>());
        service.run();
    }
    catch(std::exception const& e)
    {
        qCritical() <<e.what();
    }
}
