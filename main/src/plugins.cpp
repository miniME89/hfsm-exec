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

using namespace hfsmexec;

/*
 * CommunicationPlugin
 */
const Logger* CommunicationPlugin::logger = Logger::getLogger(LOGGER_PLUGIN);

CommunicationPlugin::CommunicationPlugin(const QString &pluginId) :
    pluginId(pluginId) {

}

CommunicationPlugin::~CommunicationPlugin() {

}

const QString& CommunicationPlugin::getPluginId() const {
    return pluginId;
}

void CommunicationPlugin::success(const Value& output) {
    if (successCallback) {
        successCallback(output);
    }
}

void CommunicationPlugin::error(QString message) {
    if (errorCallback) {
        errorCallback(message);
    }
}

/*
 * ImporterPlugin
 */
const Logger* ImporterPlugin::logger = Logger::getLogger(LOGGER_PLUGIN);

ImporterPlugin::ImporterPlugin(const QString& pluginId) :
    pluginId(pluginId) {

}

ImporterPlugin::~ImporterPlugin() {

}

const QString& ImporterPlugin::getPluginId() const {
    return pluginId;
}

/*
 * ExporterPlugin
 */
const Logger* ExporterPlugin::logger = Logger::getLogger(LOGGER_PLUGIN);

ExporterPlugin::ExporterPlugin(const QString& pluginId) :
    pluginId(pluginId) {

}

ExporterPlugin::~ExporterPlugin() {

}

const QString& ExporterPlugin::getPluginId() const {
    return pluginId;
}

/*
 * PluginLoader
 */
const Logger* PluginLoader::logger = Logger::getLogger(LOGGER_PLUGIN);

PluginLoader::PluginLoader() {

}

PluginLoader::~PluginLoader() {

}

CommunicationPlugin* PluginLoader::getCommunicationPlugin(const QString& pluginId) {
    QMap<QString, CommunicationPlugin*>::Iterator it = communicationPlugins.find(pluginId);

    if (it == communicationPlugins.end()) {
        logger->warning(QString("couldn't get communication plugin \"%1\"").arg(pluginId));

        return NULL;
    }

    return it.value()->create();
}

ImporterPlugin* PluginLoader::getImporterPlugin(const QString& pluginId) {
    QMap<QString, ImporterPlugin*>::Iterator it = importerPlugins.find(pluginId);

    if (it == importerPlugins.end()) {
        logger->warning(QString("couldn't get importer plugin \"%1\"").arg(pluginId));

        return NULL;
    }

    return it.value();
}

ExporterPlugin* PluginLoader::getExporterPlugin(const QString& pluginId) {
    QMap<QString, ExporterPlugin*>::Iterator it = exporterPlugins.find(pluginId);

    if (it == exporterPlugins.end()) {
        logger->warning(QString("couldn't get exporter plugin \"%1\"").arg(pluginId));

        return NULL;
    }

    return it.value();
}

bool PluginLoader::load(const QString &path) {
    QDir pluginsDir = QDir(path);
    pluginsDir.setNameFilters(QStringList("*.so"));

    if (!pluginsDir.exists()) {
        logger->warning(QString("couldn't load plugins in directory \"%1\": directory doesn't exist").arg(path));

        return false;
    }

    logger->info(QString("loading all plugins in directory %1").arg(pluginsDir.absolutePath()));

    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QString filePath = pluginsDir.absoluteFilePath(fileName);

        logger->info(QString("load plugin from file %1").arg(filePath));

        // load plugin
        QPluginLoader pluginLoader(filePath);
        QObject* plugin = pluginLoader.instance();
        if (!plugin) {
            logger->warning(QString("invalid plugin: %1").arg(pluginLoader.errorString()));

            continue;
        }

        // communication plugin
        CommunicationPlugin* instanceCommunicationPlugin = qobject_cast<CommunicationPlugin*>(plugin);
        if (instanceCommunicationPlugin) {
            QString pluginId = instanceCommunicationPlugin->getPluginId();

            communicationPlugins[pluginId] = instanceCommunicationPlugin;

            logger->info(QString("successfully loaded communication plugin \"%1\"").arg(pluginId));

            continue;
        }

        // importer plugin
        ImporterPlugin* instanceImporterPlugin = qobject_cast<ImporterPlugin*>(plugin);
        if (instanceImporterPlugin) {
            QString pluginId = instanceImporterPlugin->getPluginId();

            importerPlugins[pluginId] = instanceImporterPlugin;

            logger->info(QString("successfully loaded importer plugin \"%1\"").arg(pluginId));

            continue;
        }

        // exporter plugin
        ExporterPlugin* instanceExporterPlugin = qobject_cast<ExporterPlugin*>(plugin);
        if (instanceExporterPlugin) {
            QString pluginId = instanceExporterPlugin->getPluginId();

            exporterPlugins[pluginId] = instanceExporterPlugin;

            logger->info(QString("successfully loaded exporter plugin \"%1\"").arg(pluginId));

            continue;
        }

        logger->warning("couldn't load plugin: unknown plugin instance");
    }

    return true;
}
