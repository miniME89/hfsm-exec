#include <plugins.h>
#include <QDebug>
#include <QDir>
#include <QPluginLoader>
#include <QCoreApplication>

using namespace hfsmexec;

/*
 * InvokeStatePluginLoader
 */
InvokeStatePluginLoader::InvokeStatePluginLoader()
{
}

bool InvokeStatePluginLoader::load(QMap<QString, InvokeStatePlugin*>& plugins)
{
    return load(QCoreApplication::instance()->applicationDirPath(), plugins);
}

bool InvokeStatePluginLoader::load(const QString& path, QMap<QString, InvokeStatePlugin*>& plugins)
{
    QDir pluginsDir = QDir(path);
    pluginsDir.setNameFilters(QStringList("*.so"));

    qDebug() <<"loading all plugins in directory" <<pluginsDir.path();

    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QString filePath = pluginsDir.absoluteFilePath(fileName);

        //load plugin
        QPluginLoader pluginLoader(filePath);
        QObject* plugin = pluginLoader.instance();
        if (!plugin)
        {
            qDebug() <<"invalid plugin:" <<pluginLoader.errorString();

            continue;
        }

        //cast plugin
        InvokeStatePlugin* instance = qobject_cast<InvokeStatePlugin*>(plugin);
        if (!instance)
        {
            qDebug() <<"invalid plugin: plugin is not of type" <<QString("InvokeStatePlugin");

            continue;
        }

        QString pluginId = instance->getPluginId();

        //validate plugin id
        if (pluginId.isEmpty())
        {
            qDebug() <<"invalid plugin id";

            continue;
        }

        //verify unique plugin id
        if (plugins.contains(pluginId))
        {
            qDebug() <<"invalid plugin id: plugin with plugin id" <<pluginId <<"already loaded";

            continue;
        }

        plugins[pluginId] = instance;

        qDebug() <<"loaded plugin" <<pluginId;
    }

    return true;
}

/*
 * InvokeStatePlugin
 */
const QString& InvokeStatePlugin::getPluginId() const
{
    return pluginId;
}

/*
 * InvokeStatePluginLoaderTest
 */
InvokeStatePluginLoaderTest::InvokeStatePluginLoaderTest()
{
    InvokeStatePluginLoader loader;
    QMap<QString, InvokeStatePlugin*> plugins;
    loader.load("/home/marcel/Programming/hfsm-exec/plugins/build-plugin-rest-Desktop-Debug/", plugins);
}
