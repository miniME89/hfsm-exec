#include <application.h>

using namespace hfsmexec;

/*
 * main
 */
int main(int argc, char** argv)
{
    Application* application = new Application(argc, argv);

    return application->exec();
}
