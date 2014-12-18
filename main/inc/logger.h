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

#define LOG_API "api"
#define LOG_APPLICATION "application"
#define LOG_DECODER "decoder"
#define LOG_PLUGIN "plugin"
#define LOG_STATEMACHINE "statemachine"
#define LOG_BUILDER "builder"
#define LOG_VALUE "value"
#define LOG_UTILS "utils"

#include <QString>

namespace hfsmexec
{
    enum Level {
        Global = 1,
        Trace = 2,
        Debug = 4,
        Fatal = 8,
        Error = 16,
        Warning = 32,
        Verbose = 64,
        Info = 128,
        Unknown = 1010
    };

    class Logger
    {
        public:
            Logger();
            ~Logger();

            void setLoggerEnabled(bool enabled);
            void setLoggerEnabled(Level level, bool enabled);
            void setLoggerEnabled(const QString& loggerId, bool enabled);
            void setLoggerEnabled(const QString& loggerId, Level level, bool enabled);

            void setFileOut(bool enabled);
            void setFilename(const QString& filename);

            void setConsoleOut(bool enabled);
    };
}

#endif
