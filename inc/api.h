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

#include <parameter_server.h>

#include <cppcms/application.h>
#include <cppcms/service.h>
#include <cppcms/applications_pool.h>

namespace hfsmexec
{
    class Api
    {
        public:
            static Api& getInstance();

            ~Api();

            bool getParameter(const std::string& path, std::string& json);
            bool setParameter(const std::string& path, const std::string& json);
            bool deleteParameter(const std::string& path);

        private:
            static Api* instance;
            ParameterServer parameterServer; //TODO temp

            Api();
    };

    class WebApi : public cppcms::application {
        public:
            WebApi(cppcms::service &srv);

            virtual void main(std::string url);

            std::string content();

            void handlerParameters(std::string path);
            void handlerParametersGet(std::string path);
            void handlerParametersPut(std::string path);
            void handlerParametersDelete(std::string path);
    };

    class WebApiTest
    {
        public:
            WebApiTest();
    };
}

#endif
