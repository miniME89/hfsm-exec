#ifndef PLUGIN_REST_H
#define PLUGIN_REST_H

#include <statemachine.h>
#include <plugins.h>

#include <QtPlugin>

class RestInvokePlugin : public QObject, public hfsmexec::InvokeStatePlugin
{
    Q_OBJECT
    Q_INTERFACES(hfsmexec::InvokeStatePlugin)

    public:
        RestInvokePlugin();
        virtual ~RestInvokePlugin();

        virtual bool invoke();
        virtual bool cancel();
};

#endif
