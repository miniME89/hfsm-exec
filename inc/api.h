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
