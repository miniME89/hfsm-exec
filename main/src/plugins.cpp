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

#include <QDir>
#include <QPluginLoader>

using namespace hfsmexec;

/*
 * CommunicationPlugin
 */
const Logger* CommunicationPlugin::logger = Logger::getLogger(LOGGER_PLUGIN);

CommunicationPlugin::CommunicationPlugin(const QString &pluginId) :
    pluginId(pluginId)
{

}

CommunicationPlugin::~CommunicationPlugin()
{

}

QString CommunicationPlugin::getPluginId() const
{
    return pluginId;
}

/*
 * CommunicationPluginLoader
 */
const Logger* CommunicationPluginLoader::logger = Logger::getLogger(LOGGER_PLUGIN);

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
        logger->warning(QString("couldn't load communication plugins in directory \"%1\": directory doesn't exist").arg(path));

        return false;
    }

    logger->info(QString("loading all communication plugins in directory %1").arg(pluginsDir.absolutePath()));

    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QString filePath = pluginsDir.absoluteFilePath(fileName);

        //load plugin
        QPluginLoader pluginLoader(filePath);
        QObject* plugin = pluginLoader.instance();
        if (!plugin)
        {
            logger->warning(QString("invalid communication plugin: %1").arg(pluginLoader.errorString()));

            continue;
        }

        //cast plugin
        CommunicationPlugin* instance = qobject_cast<CommunicationPlugin*>(plugin);
        if (!instance)
        {
            logger->warning("invalid communication plugin: plugin is not of type \"CommunicationPlugin\"");

            continue;
        }

        QString pluginId = instance->getPluginId();

        //validate plugin id
        if (pluginId.isEmpty())
        {
            logger->warning("invalid communication plugin id: empty communication plugin id");

            continue;
        }

        //verify unique plugin id
        for (int i = 0; plugins.size(); i++)
        {
            logger->warning(QString("invalid communication plugin id: communication plugin with plugin id \"%1\" already loaded").arg(pluginId));

            continue;
        }

        plugins.append(instance);

        logger->info(QString("loaded communication plugin with pluginId %1").arg(pluginId));
    }

    return true;
}
