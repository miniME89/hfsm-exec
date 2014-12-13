#include <api_impl.h>
#include <logger.h>
#include <application.h>

#include <easylogging++.h>

#include <QUrl>

using namespace hfsmexec;

/*
 * Api
 */
Api::Api(jsonrpc::AbstractServerConnector& connector) :
    AbstractApi(connector)
{

}

bool Api::loadStateMachine(const std::string& stateMachine)
{
    QUrl url(stateMachine.c_str());
    if (url.isValid())
    {
        return Application::getInstance()->loadStateMachine(url);
    }
    else
    {
        return Application::getInstance()->loadStateMachine(stateMachine.c_str());
    }
}

bool Api::UnloadStateMachine()
{
    return Application::getInstance()->unloadStateMachine();
}

bool Api::StartStateMachine()
{
    return Application::getInstance()->startStateMachine();
}

bool Api::StopStateMachine()
{
    return Application::getInstance()->stopStateMachine();
}

bool Api::postEvent(const std::string& event, const std::string& message, const std::string& origin)
{
    NamedEvent* e = new NamedEvent(event.c_str());
    e->setMessage(message.c_str());
    e->setOrigin(origin.c_str());

    return Application::getInstance()->postEvent(e);
}

/*
 * ApiWorker
 */
ApiWorker::ApiWorker() :
    server(new jsonrpc::HttpServer(8080)),
    api(new Api(*server))
{

}

ApiWorker::~ApiWorker()
{
    delete api;
    delete server;
}

void ApiWorker::exec()
{
    CLOG(INFO, LOG_API) <<"start http server";

    server->StartListening();
}

void ApiWorker::quit()
{
    CLOG(INFO, LOG_API) <<"stop http server";

    server->StopListening();
}
