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

const QString& CommunicationPlugin::getPluginId() const
{
    return pluginId;
}

/*
 * DecoderPlugin
 */
const Logger* DecoderPlugin::logger = Logger::getLogger(LOGGER_PLUGIN);

DecoderPlugin::DecoderPlugin(const QString& pluginId, const QString& encoding) :
    pluginId(pluginId),
    encoding(encoding)
{

}

DecoderPlugin::~DecoderPlugin()
{

}

const QString& DecoderPlugin::getPluginId() const
{
    return pluginId;
}

const QString& DecoderPlugin::getEncoding() const
{
    return encoding;
}

/*
 * PluginLoader
 */
const Logger* PluginLoader::logger = Logger::getLogger(LOGGER_PLUGIN);

PluginLoader::PluginLoader()
{

}

PluginLoader::~PluginLoader()
{

}

CommunicationPlugin* PluginLoader::getCommunicationPlugin(const QString& pluginId)
{
    return communicationPlugins.find(pluginId).value();
}

DecoderPlugin* PluginLoader::getDecoderPlugin(const QString& pluginId)
{
    return decoderPlugins.find(pluginId).value();
}

const QMap<QString, CommunicationPlugin*>& PluginLoader::getCommunicationPlugins() const
{
    return communicationPlugins;
}

const QMap<QString, DecoderPlugin*>& PluginLoader::getDecoderPlugins() const
{
    return decoderPlugins;
}

bool PluginLoader::load(const QString &path)
{
    QDir pluginsDir = QDir(path);
    pluginsDir.setNameFilters(QStringList("*.so"));

    if (!pluginsDir.exists())
    {
        logger->warning(QString("couldn't load plugins in directory \"%1\": directory doesn't exist").arg(path));

        return false;
    }

    logger->info(QString("loading all plugins in directory %1").arg(pluginsDir.absolutePath()));

    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QString filePath = pluginsDir.absoluteFilePath(fileName);

        logger->info(QString("load plugin from file %1").arg(filePath));

        //load plugin
        QPluginLoader pluginLoader(filePath);
        QObject* plugin = pluginLoader.instance();
        if (!plugin)
        {
            logger->warning(QString("invalid plugin: %1").arg(pluginLoader.errorString()));

            continue;
        }

        if (loadCommunicationPlugin(plugin))
        {

        }
        else if (loadDecoderPlugin(plugin))
        {

        }
        else
        {
            logger->warning("couldn't load plugin: unknown plugin instance");
        }
    }

    return true;
}

bool PluginLoader::loadCommunicationPlugin(QObject* plugin)
{
    //cast plugin
    CommunicationPlugin* instance = qobject_cast<CommunicationPlugin*>(plugin);
    if (!instance)
    {
        return false;
    }

    logger->info("plugin is of type \"CommunicationPlugin\"");

    //verify unique plugin id
    QString pluginId = instance->getPluginId();
    if (communicationPlugins.contains(pluginId))
    {
        logger->warning(QString("unload already loaded communication plugin with plugin id \"%1\"").arg(pluginId));

        delete communicationPlugins[pluginId];
    }

    communicationPlugins[pluginId] = instance;

    logger->info(QString("successfully loaded communication plugin with pluginId %1").arg(pluginId));

    return true;
}

bool PluginLoader::loadDecoderPlugin(QObject* plugin)
{
    //cast plugin
    DecoderPlugin* instance = qobject_cast<DecoderPlugin*>(plugin);
    if (!instance)
    {
        logger->warning("invalid decoder plugin: plugin is not of type \"DecoderPlugin\"");

        return false;
    }

    logger->info("plugin is of type \"DecoderPlugin\"");

    //verify unique plugin id
    QString pluginId = instance->getPluginId();
    if (decoderPlugins.contains(pluginId))
    {
        logger->warning(QString("unload already loaded decoder plugin with plugin id \"%1\"").arg(pluginId));

        delete decoderPlugins[pluginId];
    }

    decoderPlugins[pluginId] = instance;

    logger->info(QString("successfully loaded decoder plugin with pluginId \"%1\"").arg(pluginId));

    return true;
}
