#include <statemachine_impl.h>
#include <decoder_impl.h>
#include <parameter_server.h>
#include <api.h>
#include <plugins.h>

#include <QCoreApplication>

using namespace hfsmexec;

int main(int argc, char **argv)
{
    QCoreApplication qtApplication(argc, argv);

    //WebApiTest;
    //new StateMachineTest;
    //XmlDecoderFactoryTest f;
    InvokeStatePluginLoaderTest t;
    //ParameterServerTest p;

    return 0; //TODO
    return qtApplication.exec();
}
