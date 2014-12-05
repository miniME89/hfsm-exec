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
#include <logger.h>
#include <application.h>

#include <easylogging++.h>

#include <QDir>
#include <QPluginLoader>

using namespace hfsmexec;

/*
 * CommunicationPlugin
 */
CommunicationPluginLoader::CommunicationPluginLoader()
{

}

CommunicationPluginLoader::~CommunicationPluginLoader()
{

}

CommunicationPlugin* CommunicationPluginLoader::getPlugin(const QString& pluginId)
{
    for (int i = 0; i < plugins.size(); i++)
    {
        if (plugins[i]->getPluginId() == pluginId)
        {
            return plugins[i];
        }
    }

    return NULL;
}

const QList<CommunicationPlugin*>& CommunicationPluginLoader::getPlugins() const
{
    return plugins;
}

bool CommunicationPluginLoader::load(const QString &path)
{
    QDir pluginsDir = QDir(path);
    pluginsDir.setNameFilters(QStringList("*.so"));

    if (!pluginsDir.exists())
    {
        CLOG(WARNING, LOG_PLUGIN) <<"couldn't load communication plugins in directory: directory doesn't exist";

        return false;
    }

    CLOG(INFO, LOG_PLUGIN) <<"loading all communication plugins in directory " <<pluginsDir.absolutePath();

    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QString filePath = pluginsDir.absoluteFilePath(fileName);

        //load plugin
        QPluginLoader pluginLoader(filePath);
        QObject* plugin = pluginLoader.instance();
        if (!plugin)
        {
            CLOG(WARNING, LOG_PLUGIN) <<"invalid communication plugin: " <<pluginLoader.errorString();

            continue;
        }

        //cast plugin
        CommunicationPlugin* instance = qobject_cast<CommunicationPlugin*>(plugin);
        if (!instance)
        {
            CLOG(WARNING, LOG_PLUGIN) <<"invalid communication plugin: plugin is not of type \"CommunicationPlugin\"";

            continue;
        }

        QString pluginId = instance->getPluginId();

        //validate plugin id
        if (pluginId.isEmpty())
        {
            CLOG(WARNING, LOG_PLUGIN) <<"invalid communication plugin id: empty communication plugin id";

            continue;
        }

        //verify unique plugin id
        for (int i = 0; plugins.size(); i++)
        {
            CLOG(WARNING, LOG_PLUGIN) <<"invalid communication plugin id: communication plugin with plugin id \"" <<pluginId <<"\" already loaded";

            continue;
        }

        plugins.append(instance);

        CLOG(INFO, LOG_PLUGIN) <<"loaded communication plugin with pluginId " <<pluginId;
    }

    return true;
}

/*
 * CommunicationPluginLoaderTest
 */
CommunicationPluginLoaderTest::CommunicationPluginLoaderTest()
{
    CommunicationPluginLoader loader;
    QList<CommunicationPlugin*> plugins;
    loader.load("plugins");
    plugins = loader.getPlugins();
}
