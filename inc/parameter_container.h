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

#ifndef PARAMETER_CONTAINER_H
#define PARAMETER_CONTAINER_H

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

    class ParameterContainer
    {
        public:
            enum XmlFormat
            {
                NAME_TAG,
                PARAMETER_TAG
            };

            ParameterContainer();
            ParameterContainer(const ParameterContainer& other);
            ~ParameterContainer();

            template<typename T>
            T get(const QString& path, T defaultValue) const;
            Bool get(const QString& path, Bool defaultValue) const;
            Number get(const QString& path, Number defaultValue) const;
            String get(const QString& path, String defaultValue) const;
            Array get(const QString& path, Array defaultValue) const;
            Object get(const QString& path, Object defaultValue) const;
            Value get(const QString& path, Value defaultValue) const;

            template<typename T>
            bool get(const QString& path, T& value, T defaultValue) const;
            bool get(const QString& path, Bool& value, Bool defaultValue) const;
            bool get(const QString& path, Number& value, Number defaultValue) const;
            bool get(const QString& path, String& value, String defaultValue) const;
            bool get(const QString& path, Array& value, Array defaultValue) const;
            bool get(const QString& path, Object& value, Object defaultValue) const;
            bool get(const QString& path, Value& value, Value defaultValue) const;

            template<typename T>
            void set(const QString& path, const T& value);
            void set(const QString& path, const Bool& value);
            void set(const QString& path, const Number& value);
            void set(const QString& path, const String& value);
            void set(const QString& path, const Array& value);
            void set(const QString& path, const Object& value);
            void set(const QString& path, const Value& value);

            void remove(const QString& path);

            bool toXml(const QString& path, QString& xml, XmlFormat format = NAME_TAG) const;
            bool toJson(const QString& path, QString& json) const;
            bool toYaml(const QString& path, QString& yaml) const;

            bool fromXml(const QString& path, const QString& xml, XmlFormat format = NAME_TAG);
            bool fromJson(const QString& path, const QString& json);
            bool fromYaml(const QString& path, const QString& yaml);

        private:
            static const QString typeName[7];

            Value* rootValue;

            bool find(const QString& path, Value*& value);
            bool find(const QString& path, Value const*& value) const;

            bool fromXml(Value& value, QDomElement& element, XmlFormat format = NAME_TAG);
    };

    class ParameterServerTest
    {
        public:
            ParameterServerTest();
    };
}

#endif
