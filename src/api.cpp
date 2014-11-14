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

#include <QDebug>

#include <cppcms/http_response.h>
#include <cppcms/http_request.h>
#include <cppcms/http_context.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>

using namespace hfsmexec;

/*
 * WebApi
 */
WebApi::WebApi(cppcms::service &srv) :
    cppcms::application(srv)
{
    dispatcher().assign("/parameters/(.*)", &WebApi::handlerParameters, this, 1);
    mapper().assign("parameters", "/parameters/{1}");
}

void WebApi::main(std::string url)
{
    qDebug() <<QString(request().request_method().c_str()) <<QString(url.c_str());
    cppcms::application::main(url);
}

std::string WebApi::content()
{
    std::pair<void*, size_t> body = request().raw_post_data();

    return std::string((const char *)body.first, body.second);
}

void WebApi::handlerParameters(std::string path)
{
    if (request().request_method() == "GET")
    {
        handlerParametersGet(path);
    }
    else if (request().request_method() == "PUT")
    {
        handlerParametersPut(path);
    }
    else if (request().request_method() == "DELETE")
    {
        handlerParametersDelete(path);
    }
    else
    {
        response().make_error_response(403);
    }
}

void WebApi::handlerParametersGet(std::string path)
{
    std::string parameters;
    if (Api::getInstance().getParameter(path, parameters))
    {
        response().out() <<parameters;
    }
    else
    {

    }
}

void WebApi::handlerParametersPut(std::string path)
{
    Api::getInstance().setParameter(path, content());
}

void WebApi::handlerParametersDelete(std::string path)
{
    Api::getInstance().deleteParameter(path);
}

/*
 * Api
 */
Api* Api::instance = NULL;

Api& Api::getInstance()
{
    if (instance == NULL)
    {
        instance = new Api();
    }

    return *instance;
}

Api::Api()
{
}

Api::~Api()
{
}

bool Api::getParameter(const std::string& path, std::string& json)
{
    json = parameterServer.toJson(QString(path.c_str())).toStdString();

    return true;
}

bool Api::setParameter(const std::string& path, const std::string& json)
{
    return parameterServer.fromJson(QString(path.c_str()), QString(json.c_str()));
}

bool Api::deleteParameter(const std::string& path)
{
    parameterServer.deleteParameter(QString(path.c_str()));

    return true;
}

/*
 * WebApiTest
 */
WebApiTest::WebApiTest()
{
    try
        {
            cppcms::json::value config;
            config["service"]["api"] = "http";
            config["service"]["port"] = 8080;

            cppcms::service srv(config);
            srv.applications_pool().mount(cppcms::applications_factory<WebApi>());
            srv.run();
        }
        catch(std::exception const &e)
        {
            std::cerr << e.what() << std::endl;
        }
}
