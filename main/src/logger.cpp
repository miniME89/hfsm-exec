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

#include <logger.h>
#include <application.h>

#include <easylogging++.h>

using namespace hfsmexec;

_INITIALIZE_EASYLOGGINGPP

/*
 * Logger
 */
Logger::Logger(const QString& name) :
    name(name)
{
    el::Loggers::getLogger(name.toStdString());
}

Logger* Logger::getLogger(const QString& name)
{
    return new Logger(name);
}

Logger::~Logger()
{

}

void Logger::info(const QString& message) const
{
    CLOG(INFO, name.toStdString().c_str()) <<message;
}

void Logger::warning(const QString& message) const
{
    CLOG(WARNING, name.toStdString().c_str()) <<message;
}

void Logger::error(const QString& message) const
{
    CLOG(ERROR, name.toStdString().c_str()) <<message;
}

void Logger::fatal(const QString& message) const
{
    CLOG(FATAL, name.toStdString().c_str()) <<message;
}

void Logger::debug(const QString& message) const
{
    CLOG(DEBUG, name.toStdString().c_str()) <<message;
}

/*
 * LoggerController
 */
LoggerController::LoggerController()
{

}

LoggerController::~LoggerController()
{

}

void LoggerController::setLoggerEnabled(bool enabled)
{
    el::Configurations config;
    config.set(el::Level::Global, el::ConfigurationType::Enabled, (enabled) ? "true" : "false");
    el::Loggers::reconfigureAllLoggers(config);
}

void LoggerController::setLoggerEnabled(const QString& name, bool enabled)
{
    el::Configurations config;
    config.set(el::Level::Global, el::ConfigurationType::Enabled, (enabled) ? "true" : "false");
    el::Loggers::reconfigureLogger(name.toStdString(), config);
}

void LoggerController::setFileOut(bool enabled)
{
    el::Configurations config;
    config.set(el::Level::Global, el::ConfigurationType::ToFile, (enabled) ? "true" : "false");
    el::Loggers::reconfigureAllLoggers(config);
}

void LoggerController::setFilename(const QString& filename)
{
    el::Configurations config;
    config.set(el::Level::Global, el::ConfigurationType::Filename, filename.toStdString());
    el::Loggers::reconfigureAllLoggers(config);
}

void LoggerController::setConsoleOut(bool enabled)
{
    el::Configurations config;
    config.set(el::Level::Global, el::ConfigurationType::ToStandardOutput, (enabled) ? "true" : "false");
    el::Loggers::reconfigureAllLoggers(config);
}
