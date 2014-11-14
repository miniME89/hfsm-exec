#include "plugin_rest.h"
#include <QDebug>

RestInvokePlugin::RestInvokePlugin()
{
    pluginId = "REST";
}

RestInvokePlugin::~RestInvokePlugin()
{

}

bool RestInvokePlugin::invoke()
{
    qDebug() <<"invoke";

    return true;
}

bool RestInvokePlugin::cancel()
{
    qDebug() <<"cancel";

    return true;
}

Q_EXPORT_PLUGIN2(plugin-rest, RestInvokePlugin)
