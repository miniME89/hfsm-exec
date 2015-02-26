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

#ifndef VALUE_H
#define VALUE_H

#define LOGGER_VALUE "value"

#include <logger.h>

#include <QObject>
#include <QString>
#include <QMutex>
#include <QScriptEngine>
#include <QScriptClass>

#include <exception>

namespace pugi
{
    class xml_node;
}

namespace Json
{
    class Value;
}

namespace YAML
{
    class Node;
}

namespace hfsmexec
{
    class ArbitraryValue;

    class ValueException : public std::exception
    {
        public:
            ValueException();
            ValueException(QString const& message);
            virtual ~ValueException() throw();

            virtual const char* what() const throw();

        private:
            QString message;
    };

    class Value : public QObject
    {
        Q_OBJECT

        public:
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
            } ValueType;

            struct Undefined {};
            struct Null {};
            typedef bool Boolean;
            typedef int Integer;
            typedef double Float;
            typedef QString String;
            typedef QList<Value> Array;
            typedef QMap<String, Value> Object;

            Value();
            Value(const Boolean& value);
            Value(const Integer& value);
            Value(const Float& value);
            Value(const char* value);
            Value(const String& value);
            Value(const Array& value);
            Value(const Object& value);
            Value(const Value& value);
            Value(Value* const & value);
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

            Value& getValue(const QString& path);
            const Value& getValue(const QString& path) const;
            Value& getValue(int i);
            const Value& getValue(int i) const;

            int size();
            void remove(const QString& key);
            void remove(int i);
            bool contains(const QString& key);

            void undefined();
            void null();

            const ValueType& getType() const;

            String toString();

            bool toXml(QString& xml) const;
            bool toJson(QString& json) const;
            bool toYaml(QString& yaml) const;

            bool fromXml(const QString& xml);
            bool fromJson(const QString& json);
            bool fromYaml(const QString& yaml);

            const Value& operator=(const Boolean& value);
            const Value& operator=(const Integer& value);
            const Value& operator=(const Float& value);
            const Value& operator=(const char* value);
            const Value& operator=(const String& value);
            const Value& operator=(const Array& value);
            const Value& operator=(const Object& value);
            const Value& operator=(const Value& other);
            const Value& operator=(const Value* other);

            bool operator==(const Value& other) const;
            bool operator!=(const Value& other) const;

            Value& operator[](const QString& path);
            const Value& operator[](const QString& path) const;
            Value& operator[](int i);
            const Value& operator[](int i) const;

        private:
            static const Logger* logger;
            ArbitraryValue* value;

            template <typename T>
            bool getValue(T& value, T defaultValue) const;

            template <typename T>
            void setValue(const T& value);

            bool buildToXml(const Value* value, pugi::xml_node* xmlValue) const;
            bool buildToJson(const Value* value, Json::Value* jsonValue) const;
            bool buildToYaml(const Value* value, YAML::Node* yamlValue) const;

            bool buildFromXml(Value* value, pugi::xml_node* xmlValue);
            bool buildFromJson(Value* value, Json::Value* jsonValue);
            bool buildFromYaml(Value* value, YAML::Node* yamlValue);
    };

    class ArbitraryValue
    {
        public:
            ArbitraryValue();
            ArbitraryValue(ArbitraryValue const &other);
            template<typename T>
            ArbitraryValue(T const &v);
            ~ArbitraryValue();

            const Value::ValueType& getType() const;

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

            void incReference();
            void decReference();

        private:
            int refCounter;
            QMutex mutexRefCounter;

            Value::ValueType type;

            union DataUnion
            {
                void* p;
                bool b;
                int i;
                double d;
                char s[sizeof(Value::String)];
                char a[sizeof(Value::Array)];
                char o[sizeof(Value::Object)];
            } data;

            template<typename T>
            void create(T const &v);
            void create(Value::ValueType t);
            void create(Value::ValueType t, DataUnion const& other);

            void destroy();
    };

    class ValueScriptBinding : public QScriptClass
    {
        public:
            virtual QScriptValue property(const QScriptValue& object, const QScriptString& name, uint id);
            virtual void setProperty(QScriptValue& object, const QScriptString& name, uint id, const QScriptValue& newValue);
            virtual QueryFlags queryProperty(const QScriptValue& object, const QScriptString& name, QueryFlags flags, uint* id);

            static QScriptValue create(QScriptEngine* engine, Value* value);

        private:
            ValueScriptBinding(QScriptEngine* engine);
            ~ValueScriptBinding();

            void setPropertyFromArray(const QVariantList& list, Value* value);
            void setPropertyFromObject(const QVariantMap& map, Value* value);
    };
}

Q_DECLARE_METATYPE(hfsmexec::Value*)

#endif
