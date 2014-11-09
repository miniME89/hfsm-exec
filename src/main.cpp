#include <statemachine.h>
#include <parameter_server.h>
#include <api.h>

#include <QCoreApplication>

using namespace hfsmexec;

int main(int argc, char **argv)
{
    QCoreApplication qtApplication(argc, argv);

    WebApiTest();

    return 0; //TODO
    return qtApplication.exec();
}
