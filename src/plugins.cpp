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

#include <plugins.h>
#include <QDebug>
#include <QDir>
#include <QPluginLoader>
#include <QCoreApplication>

using namespace hfsmexec;

/*
 * CommunicationPluginLoader
 */
CommunicationPluginLoader::CommunicationPluginLoader()
{
}

bool CommunicationPluginLoader::load(QMap<QString, CommunicationPlugin*>& plugins)
{
    return load(QCoreApplication::instance()->applicationDirPath(), plugins);
}

bool CommunicationPluginLoader::load(const QString& path, QMap<QString, CommunicationPlugin*>& plugins)
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
            qWarning() <<"invalid plugin:" <<pluginLoader.errorString();

            continue;
        }

        //cast plugin
        CommunicationPlugin* instance = qobject_cast<CommunicationPlugin*>(plugin);
        if (!instance)
        {
            qWarning() <<"invalid plugin: plugin is not of type" <<QString("CommunicationPlugin");

            continue;
        }

        QString pluginId = instance->getPluginId();

        //validate plugin id
        if (pluginId.isEmpty())
        {
            qWarning() <<"invalid plugin id";

            continue;
        }

        //verify unique plugin id
        if (plugins.contains(pluginId))
        {
            qWarning() <<"invalid plugin id: plugin with plugin id" <<pluginId <<"already loaded";

            continue;
        }

        plugins[pluginId] = instance;

        qDebug() <<"loaded plugin" <<pluginId;
    }

    return true;
}

/*
 * CommunicationPlugin
 */
const QString& CommunicationPlugin::getPluginId() const
{
    return pluginId;
}

/*
 * CommunicationPluginLoaderTest
 */
CommunicationPluginLoaderTest::CommunicationPluginLoaderTest()
{
    CommunicationPluginLoader loader;
    QMap<QString, CommunicationPlugin*> plugins;
    loader.load("/home/marcel/Programming/hfsm-exec/plugins/build-plugin-rest-Desktop-Debug/", plugins);
}
