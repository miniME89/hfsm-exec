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

#ifndef VALUECONTAINER_H
#define VALUECONTAINER_H

#define LOGGER_VALUE "value"

#include <logger.h>

#include <QString>
#include <QList>
#include <QMap>

#include <exception>

namespace hfsmexec
{
    class Value;

    typedef enum
    {
        TYPE_UNDEFINED,
        TYPE_NULL,
        TYPE_BOOLEAN,
        TYPE_INTEGER,
        TYPE_FLOAT,
        TYPE_STRING,
        TYPE_ARRAY,
        TYPE_OBJECT
    } ArbitraryValueType;

    struct Undefined {};
    struct Null {};
    typedef bool Boolean;
    typedef int Integer;
    typedef double Float;
    typedef QString String;
    typedef QList<Value> Array;
    typedef QMap<String, Value> Object;

    class ArbitraryValueException : public std::exception
    {
        public:
            ArbitraryValueException();
            ArbitraryValueException(QString const& message);
            virtual ~ArbitraryValueException() throw();

            virtual const char* what() const throw();

        private:
            QString message;
    };

    class ArbitraryValue
    {
        public:
            ArbitraryValue();
            ArbitraryValue(ArbitraryValue const &other);
            template<typename T>
            ArbitraryValue(T const &v);
            ~ArbitraryValue();

            const ArbitraryValueType& getType() const;

            void* ptr();
            void const* ptr() const;

            template<typename T>
            T& get();
            template<typename T>
            T const& get() const;

            template<typename T>
            void set(T const& other);
            void set(ArbitraryValue const& other);

            bool operator==(ArbitraryValue const& other) const;

        private:
            ArbitraryValueType type;

            union DataUnion
            {
                void* p;
                bool b;
                int i;
                double d;
                char s[sizeof(String)];
                char a[sizeof(Array)];
                char o[sizeof(Object)];
            } data;

            template<typename T>
            void create(T const &v);
            void create(ArbitraryValueType t);
            void create(ArbitraryValueType t, DataUnion const& other);

            void destroy();
    };

    class Value
    {
        public:
            enum XmlFormat
            {
                NAME_TAG,
                PARAMETER_TAG
            };

            Value();
            Value(const Value& value);
            template<typename T>
            Value(const T& value);
            ~Value();

            bool isUndefined() const;
            bool isNull() const;
            bool isBoolean() const;
            bool isInteger() const;
            bool isFloat() const;
            bool isString() const;
            bool isArray() const;
            bool isObject() const;

            Boolean getBoolean(Boolean defaultValue = false) const;
            Integer getInteger(Integer defaultValue = 0) const;
            Float getFloat(Float defaultValue = 0) const;
            String getString(String defaultValue = "") const;
            Array getArray(Array defaultValue = Array()) const;
            Object getObject(Object defaultValue = Object()) const;

            bool get(Boolean& value, Boolean defaultValue = false) const;
            bool get(Integer& value, Integer defaultValue = 0) const;
            bool get(Float& value, Float defaultValue = 0) const;
            bool get(String& value, String defaultValue = "") const;
            bool get(Array& value, Array defaultValue = Array()) const;
            bool get(Object& value, Object defaultValue = Object()) const;

            void set(const Boolean& value);
            void set(const Integer& value);
            void set(const Float& value);
            void set(const char* value);
            void set(const String& value);
            void set(const Array& value);
            void set(const Object& value);

            void undefined();
            void null();

            const ArbitraryValueType& getType() const;

            bool toXml(QString& xml) const;
            bool toJson(QString& json) const;
            bool toYaml(QString& yaml) const;

            bool fromXml(const QString& xml);
            bool fromJson(const QString& json);
            bool fromYaml(const QString& yaml);

            const Value& operator=(const Value& other);
            bool operator==(const Value& other) const;
            Value& operator[](const QString& path);
            const Value& operator[](const QString& path) const;
            Value& operator[](int i);
            const Value& operator[](int i) const;

        private:
            static const Logger* logger;
            ArbitraryValue value;

            template <typename T>
            bool getValue(T& value, T defaultValue) const;

            template <typename T>
            void setValue(const T& value);

            bool buildToXml(const Value* value, void* data) const;
            bool buildToJson(const Value* value, void* data) const;
            bool buildToYaml(const Value* value, void* data) const;

            bool buildFromXml(Value* value, void* data);
            bool buildFromJson(Value* value, void* data);
            bool buildFromYaml(Value* value, void* data);
    };
}

#endif
