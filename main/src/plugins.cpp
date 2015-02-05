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
 * ImporterPlugin
 */
const Logger* ImporterPlugin::logger = Logger::getLogger(LOGGER_PLUGIN);

ImporterPlugin::ImporterPlugin(const QString& pluginId) :
    pluginId(pluginId)
{

}

ImporterPlugin::~ImporterPlugin()
{

}

const QString& ImporterPlugin::getPluginId() const
{
    return pluginId;
}

/*
 * ExporterPlugin
 */
const Logger* ExporterPlugin::logger = Logger::getLogger(LOGGER_PLUGIN);

ExporterPlugin::ExporterPlugin(const QString& pluginId) :
    pluginId(pluginId)
{

}

ExporterPlugin::~ExporterPlugin()
{

}

const QString& ExporterPlugin::getPluginId() const
{
    return pluginId;
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

const QMap<QString, CommunicationPlugin*>& PluginLoader::getCommunicationPlugins() const
{
    return communicationPlugins;
}

ImporterPlugin* PluginLoader::getImporterPlugin(const QString& pluginId)
{
    return importerPlugins.find(pluginId).value();
}

const QMap<QString, ImporterPlugin*>& PluginLoader::getImporterPlugins() const
{
    return importerPlugins;
}

ExporterPlugin* PluginLoader::getExporterPlugin(const QString& pluginId)
{
    return exporterPlugins.find(pluginId).value();
}

const QMap<QString, ExporterPlugin*>& PluginLoader::getExporterPlugins() const
{
    return exporterPlugins;
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
        else if (loadImporterPlugin(plugin))
        {

        }
        else if (loadExporterPlugin(plugin))
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

    //verify unique plugin id
    QString pluginId = instance->getPluginId();
    if (communicationPlugins.contains(pluginId))
    {
        logger->warning(QString("unload already loaded communication plugin \"%1\"").arg(pluginId));

        delete communicationPlugins[pluginId];
    }

    communicationPlugins[pluginId] = instance;

    logger->info(QString("successfully loaded communication plugin \"%1\"").arg(pluginId));

    return true;
}

bool PluginLoader::loadImporterPlugin(QObject* plugin)
{
    //cast plugin
    ImporterPlugin* instance = qobject_cast<ImporterPlugin*>(plugin);
    if (!instance)
    {
        return false;
    }

    //verify unique plugin id
    QString pluginId = instance->getPluginId();
    if (importerPlugins.contains(pluginId))
    {
        logger->warning(QString("unload already loaded importer plugin \"%1\"").arg(pluginId));

        delete importerPlugins[pluginId];
    }

    importerPlugins[pluginId] = instance;

    logger->info(QString("successfully loaded importer plugin \"%1\"").arg(pluginId));

    return true;
}

bool PluginLoader::loadExporterPlugin(QObject* plugin)
{
    //cast plugin
    ExporterPlugin* instance = qobject_cast<ExporterPlugin*>(plugin);
    if (!instance)
    {
        return false;
    }

    //verify unique plugin id
    QString pluginId = instance->getPluginId();
    if (exporterPlugins.contains(pluginId))
    {
        logger->warning(QString("unload already loaded exporter plugin \"%1\"").arg(pluginId));

        delete exporterPlugins[pluginId];
    }

    exporterPlugins[pluginId] = instance;

    logger->info(QString("successfully loaded exporter plugin \"%1\"").arg(pluginId));

    return true;
}
