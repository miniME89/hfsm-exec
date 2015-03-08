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
#define ELPP_QT_LOGGING
#define ELPP_NO_DEFAULT_LOG_FILE
#define ELPP_STACKTRACE_ON_CRASH

#include <logger.h>
#include <application.h>

#include <easylogging++.h>

using namespace hfsmexec;

INITIALIZE_EASYLOGGINGPP

/*
 * Logger
 */
QMap<QString, Logger*> Logger::loggers;

Logger::Logger(const QString& name) :
    name(name) {
    el::Loggers::getLogger(name.toStdString());
}

Logger* Logger::getLogger(const QString& name) {
    if (loggers.contains(name)) {
        return loggers[name];
    }

    Logger* logger = new Logger(name);

    loggers[name] = logger;

    return logger;
}

Logger::~Logger() {

}

void Logger::info(const QString& message) const {
    CLOG(INFO, name.toStdString().c_str()) <<message;

    Value value;
    value["scope"] = name;
    value["level"] = INFO;
    value["message"] = message;
    Application::getInstance()->getApi().pushlog(value);
}

void Logger::warning(const QString& message) const {
    CLOG(WARNING, name.toStdString().c_str()) <<message;

    Value value;
    value["scope"] = name;
    value["level"] = WARNING;
    value["message"] = message;
    Application::getInstance()->getApi().pushlog(value);
}

void Logger::error(const QString& message) const {
    CLOG(ERROR, name.toStdString().c_str()) <<message;

    Value value;
    value["scope"] = name;
    value["level"] = ERROR;
    value["message"] = message;
    Application::getInstance()->getApi().pushlog(value);
}

void Logger::fatal(const QString& message) const {
    CLOG(FATAL, name.toStdString().c_str()) <<message;

    Value value;
    value["scope"] = name;
    value["level"] = FATAL;
    value["message"] = message;
    Application::getInstance()->getApi().pushlog(value);
}

void Logger::debug(const QString& message) const {
    CLOG(DEBUG, name.toStdString().c_str()) <<message;

    Value value;
    value["scope"] = name;
    value["level"] = DEBUG;
    value["message"] = message;
    Application::getInstance()->getApi().pushlog(value);
}

void Logger::setLoggerEnabled(bool enabled) {
    el::Configurations config;
    config.set(el::Level::Global, el::ConfigurationType::Enabled, (enabled) ? "true" : "false");
    el::Loggers::reconfigureAllLoggers(config);
}

void Logger::setLoggerEnabled(const QString& name, bool enabled) {
    el::Configurations config;
    config.set(el::Level::Global, el::ConfigurationType::Enabled, (enabled) ? "true" : "false");
    el::Loggers::reconfigureLogger(name.toStdString(), config);
}

void Logger::setFileOut(bool enabled) {
    el::Configurations config;
    config.set(el::Level::Global, el::ConfigurationType::ToFile, (enabled) ? "true" : "false");
    el::Loggers::reconfigureAllLoggers(config);
}

void Logger::setFilename(const QString& filename) {
    el::Configurations config;
    config.set(el::Level::Global, el::ConfigurationType::Filename, filename.toStdString());
    el::Loggers::reconfigureAllLoggers(config);
}

void Logger::setConsoleOut(bool enabled) {
    el::Configurations config;
    config.set(el::Level::Global, el::ConfigurationType::ToStandardOutput, (enabled) ? "true" : "false");
    el::Loggers::reconfigureAllLoggers(config);
}
