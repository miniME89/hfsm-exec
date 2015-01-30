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

#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QMap>

namespace hfsmexec
{
    class Logger
    {
        public:
            typedef enum Level
            {
                INFO,
                WARNING,
                ERROR,
                FATAL,
                DEBUG
            } Level;
            typedef void LogCallback(const QString& name, Level level, const QString& message);

            static Logger* getLogger(const QString& name);

            ~Logger();

            void info(const QString& message) const;
            void warning(const QString& message) const;
            void error(const QString& message) const;
            void fatal(const QString& message) const;
            void debug(const QString& message) const;

            static void registerListener(const QString& id, const std::function<LogCallback>& listener);
            static void unregisterListener(const QString& id);

            static void setLoggerEnabled(bool enabled);
            static void setLoggerEnabled(const QString& name, bool enabled);

            static void setFileOut(bool enabled);
            static void setFilename(const QString& filename);

            static void setConsoleOut(bool enabled);

        private:
            static QMap<QString, Logger*> loggers;
            static QMap<QString, std::function<LogCallback>> listeners;
            const QString name;

            Logger(const QString& name);

            void notifyListeners(const QString& name, Level level, const QString& message) const;
    };
}

#endif
