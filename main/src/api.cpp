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
#include <logger.h>
#include <application.h>

#include <easylogging++.h>

#include <cppcms/http_response.h>
#include <cppcms/http_request.h>
#include <cppcms/http_context.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>

#include <QtConcurrentRun>

using namespace hfsmexec;

static cppcms::service* serviceHandle = NULL;

static void worker()
{
    CLOG(INFO, LOG_API) <<"start http server";

    try
    {
        cppcms::json::value config;
        config["service"]["api"] = "http";
        config["service"]["port"] = 8080;
        config["service"]["disable_global_exit_handling"] = true;

        serviceHandle = new cppcms::service(config);
        serviceHandle->applications_pool().mount(cppcms::applications_factory<Api>());
        serviceHandle->run();
    }
    catch(std::exception const& e)
    {
        CLOG(FATAL, LOG_API) <<e.what();
    }

    CLOG(INFO, LOG_API) <<"stopped http server";
}

/*
 * Api
 */
Api::Api(cppcms::service &srv) :
    cppcms::application(srv)
{
    dispatcher().assign("/statemachine/event", &Api::handlerEvent, this);
    mapper().assign("events", "/statemachine/event");
}

Api::~Api()
{

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
        Application::instance()->postEvent(namedEvent);
    }
    else
    {
        response().make_error_response(403);
    }
}

void Api::main(std::string url)
{
    CLOG(INFO, LOG_API) <<"request: " <<url;

    cppcms::application::main(url);
}

std::string Api::content()
{
    std::pair<void*, size_t> body = request().raw_post_data();

    return std::string((const char *)body.first, body.second);
}

void Api::exec()
{
    QtConcurrent::run(worker);
}

void Api::quit()
{
    if (serviceHandle != NULL)
    {
        serviceHandle->shutdown();
    }
}
