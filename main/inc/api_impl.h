#ifndef API_IMPL_H
#define API_IMPL_H

#include <api.h>
#include <jsonrpccpp/server/connectors/httpserver.h>

namespace hfsmexec
{
    class Api : public AbstractApi
    {
        public:
            Api(jsonrpc::AbstractServerConnector& connector);

            virtual bool loadStateMachine(const std::string& stateMachine);
            virtual bool UnloadStateMachine();
            virtual bool StartStateMachine();
            virtual bool StopStateMachine();
            virtual bool postEvent(const std::string& event, const std::string& message, const std::string& origin);
    };

    class ApiWorker
    {
        public:
            ApiWorker();
            ~ApiWorker();

            void exec();
            void quit();

        private:
            jsonrpc::HttpServer* server;
            AbstractApi* api;
    };
}

#endif
