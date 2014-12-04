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

#include <QThread>

#include <cppcms/application.h>
#include <cppcms/service.h>
#include <cppcms/applications_pool.h>

namespace hfsmexec
{
    class Application;

    class Api : public cppcms::application
    {
        public:
            Api(cppcms::service &srv);
            virtual ~Api();

            void handlerEvent();

        private:
            std::string content();
    };

    class ApiExecutor : public QThread
    {
        public:
            ApiExecutor();
            virtual ~ApiExecutor();

        protected:
            void run();
    };
}

#endif
