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

#ifndef PARAMETER_SERVER_H
#define PARAMETER_SERVER_H

#include <QString>
#include <QtXml>

#include <cppcms/json.h>

namespace hfsmexec
{
    typedef bool Bool;
    typedef double Number;
    typedef std::string String;
    typedef cppcms::json::array Array;
    typedef cppcms::json::object Object;
    typedef cppcms::json::value Value;

    class ParameterServer
    {
        public:
            enum XmlFormat
            {
                NAME_TAG,
                PARAMETER_TAG
            };

            ParameterServer();

            template<typename T>
            bool getParameter(const QString& path, T& value);
            bool getParameter(const QString& path, Bool& value);
            bool getParameter(const QString& path, Number& value);
            bool getParameter(const QString& path, String& value);
            bool getParameter(const QString& path, Array& value);
            bool getParameter(const QString& path, Object& value);
            bool getParameter(const QString& path, Value& value);

            template<typename T>
            void setParameter(const QString& path, T const& value);
            void setParameter(const QString& path, Bool& value);
            void setParameter(const QString& path, Number& value);
            void setParameter(const QString& path, String& value);
            void setParameter(const QString& path, Array& value);
            void setParameter(const QString& path, Object& value);
            void setParameter(const QString& path, Value& value);

            void deleteParameter(const QString& path);

            bool toXml(const QString& path, QString& xml, XmlFormat format = NAME_TAG);
            bool toJson(const QString& path, QString& json);
            bool toYaml(const QString& path, QString& yaml);

            bool fromXml(const QString& path, const QString& xml, XmlFormat format = NAME_TAG);
            bool fromJson(const QString& path, const QString& json);
            bool fromYaml(const QString& path, const QString& yaml);

        private:
            static const QString typeName[7];

            Value parameters;

            bool getValue(const QString& path, Value*& value);
            bool getValue(const QString& path, Value const*& value);

            bool fromXml(Value& value, QDomElement& element, XmlFormat format = NAME_TAG);
    };

    class ParameterServerTest
    {
        public:
            ParameterServerTest();
    };
}

#endif
