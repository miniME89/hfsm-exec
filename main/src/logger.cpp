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

#include <easylogging++.h>

using namespace hfsmexec;

Logger::Logger(int argc, char** argv)
{
    _START_EASYLOGGINGPP(argc, argv);

    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);

    el::Loggers::getLogger(LOG_API);
    el::Loggers::getLogger(LOG_APPLICATION);
    el::Loggers::getLogger(LOG_DECODER);
    el::Loggers::getLogger(LOG_PLUGIN);
    el::Loggers::getLogger(LOG_STATEMACHINE);
    el::Loggers::getLogger(LOG_BUILDER);
    el::Loggers::getLogger(LOG_VALUE);

    setFilename("hfsm-exec.log");
}

Logger::~Logger()
{

}

void Logger::setLoggerEnabled(bool enabled)
{
    setLoggerEnabled(Global, enabled);
}

void Logger::setLoggerEnabled(Level level, bool enabled)
{
    el::Configurations config;
    config.set((el::Level)level, el::ConfigurationType::Enabled, (enabled) ? "true" : "false");
    el::Loggers::reconfigureAllLoggers(config);
}

void Logger::setLoggerEnabled(std::string loggerId, bool enabled)
{
    setLoggerEnabled(loggerId, Global, enabled);
}

void Logger::setLoggerEnabled(std::string loggerId, Level level, bool enabled)
{
    el::Configurations config;
    config.set((el::Level)level, el::ConfigurationType::Enabled, (enabled) ? "true" : "false");
    el::Loggers::reconfigureLogger(loggerId, config);
}

void Logger::setFileOut(bool enabled)
{
    el::Configurations config;
    config.set((el::Level)Global, el::ConfigurationType::ToStandardOutput, (enabled) ? "true" : "false");
    el::Loggers::reconfigureAllLoggers(config);
}

void Logger::setFilename(std::string filename)
{
    el::Configurations config;
    config.set((el::Level)Global, el::ConfigurationType::Filename, filename);
    el::Loggers::reconfigureAllLoggers(config);
}

void Logger::setConsoleOut(bool enabled)
{
    el::Configurations config;
    config.set((el::Level)Global, el::ConfigurationType::ToFile, (enabled) ? "true" : "false");
    el::Loggers::reconfigureAllLoggers(config);
}
