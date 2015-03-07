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
#include <memory>

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
    class NullValue;

    class Value : public QObject
    {
        Q_OBJECT

        friend class Iterator;

        public:
            typedef enum
            {
                TYPE_UNDEFINED = 0,
                TYPE_NULL,
                TYPE_BOOLEAN,
                TYPE_INTEGER,
                TYPE_FLOAT,
                TYPE_STRING,
                TYPE_ARRAY,
                TYPE_OBJECT
            } Type;

            const char* typeNames[8] = {"Undefined",
                                        "Null",
                                        "Boolean",
                                        "Integer",
                                        "Float",
                                        "String",
                                        "Array",
                                        "Object"};

            struct Undefined {};
            struct Null {};
            typedef bool Boolean;
            typedef int Integer32;
            typedef long Integer64;
            typedef Integer64 Integer;
            typedef float Float32;
            typedef double Float64;
            typedef Float64 Float;
            typedef const char* StringChar;
            typedef std::string StringStd;
            typedef QString StringQt;
            typedef StringQt String;
            typedef QList<Value> Array;
            typedef QMap<String, Value> Object;

            class ArrayIterator
            {
                public:
                    ArrayIterator(const Value& value);

                    inline int index() const { return it - array->begin(); }
                    inline Value& value() const { return *it; }

                    inline operator bool() const { return array != NULL && it != array->end(); }

                    inline ArrayIterator& operator++() { ++it; return *this; }
                    inline ArrayIterator operator++(int) { ArrayIterator r = *this; ++it; return r; }
                    inline ArrayIterator& operator+=(int i) { it += i; return *this; }

                    inline ArrayIterator& operator--() { --it; return *this; }
                    inline ArrayIterator operator--(int) { ArrayIterator r = *this; --it; return r; }
                    inline ArrayIterator& operator-=(int i) { it -= i; return *this; }

                    inline Value& operator*() const { return *it; }
                    inline Value* operator->() const { return &*it; }
                    Value& operator[](int i);

                private:
                    Array* array;
                    Array::iterator it;
            };

            class ObjectIterator
            {
                public:
                    ObjectIterator(const Value& value);

                    inline String key() const { return it.key(); }
                    inline Value& value() const { return *it; }

                    inline operator bool() const { return object != NULL && it != object->end(); }

                    inline ObjectIterator& operator++() { ++it; return *this; }
                    inline ObjectIterator operator++(int) { ObjectIterator r = *this; ++it; return r; }
                    inline ObjectIterator& operator+=(int i) { it += i; return *this; }

                    inline ObjectIterator& operator--() { --it; return *this; }
                    inline ObjectIterator operator--(int) { ObjectIterator r = *this; --it; return r; }
                    inline ObjectIterator& operator-=(int i) { it -= i; return *this; }

                    inline Value& operator*() const { return *it; }
                    inline Value* operator->() const { return &*it; }
                    Value& operator[](const String key);

                private:
                    Object* object;
                    Object::iterator it;
            };

            Value();
            Value(const Boolean& value);
            Value(const Integer32& value);
            Value(const Integer64& value);
            Value(const Float32& value);
            Value(const Float64& value);
            Value(const StringChar& value);
            Value(const StringStd& value);
            Value(const String& value);
            Value(const Array& value);
            Value(const Object& value);
            Value(const Value& value);
            Value(Value* const & value);
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
            void set(const Integer32& value);
            void set(const Integer64& value);
            void set(const Float32& value);
            void set(const Float64& value);
            void set(const StringChar& value);
            void set(const StringStd& value);
            void set(const String& value);
            void set(const Array& value);
            void set(const Object& value);
            void set(const Value& value);
            void set(Value* const & value);

            Value& getValue(const QString& path);
            const Value& getValue(const QString& path) const;

            void unite(const Value& value);

            int size();
            void remove(const QString& key);
            void remove(int i);
            bool contains(const QString& key);

            void undefined();
            void null();

            bool isValid() const;

            const Type& getType() const;

            String toString();

            bool toXml(QString& xml, bool pretty = false) const;
            bool toJson(QString& json, bool pretty = false) const;
            bool toYaml(QString& yaml) const;

            bool fromXml(const QString& xml);
            bool fromJson(const QString& json);
            bool fromYaml(const QString& yaml);

            const Value& operator=(const Boolean& value);
            const Value& operator=(const Integer32& value);
            const Value& operator=(const Integer64& value);
            const Value& operator=(const Float32& value);
            const Value& operator=(const Float64& value);
            const Value& operator=(const StringChar& value);
            const Value& operator=(const StringStd& value);
            const Value& operator=(const String& value);
            const Value& operator=(const Array& value);
            const Value& operator=(const Object& value);
            const Value& operator=(const Value& other);
            const Value& operator=(const Value* other);

            bool operator==(const Value& other) const;
            bool operator!=(const Value& other) const;

            Value& operator[](const QString& name);
            const Value& operator[](const QString& name) const;
            Value& operator[](int i);
            const Value& operator[](int i) const;

        private:
            static const Logger* logger;
            std::shared_ptr<ArbitraryValue> value;

            template <typename T>
            bool get(T& value) const;

            template <typename T>
            void set(const T& value);

            bool buildToXml(const Value* value, pugi::xml_node* xmlValue) const;
            bool buildToJson(const Value* value, Json::Value* jsonValue) const;
            bool buildToYaml(const Value* value, YAML::Node* yamlValue) const;

            bool buildFromXml(Value* value, pugi::xml_node* xmlValue);
            bool buildFromJson(Value* value, Json::Value* jsonValue);
            bool buildFromYaml(Value* value, YAML::Node* yamlValue);
    };

    class NullValue : public Value
    {
        public:
            static NullValue& ref();

            const Value& operator[](const QString& name) const;
            const Value& operator[](int i) const;

        private:
            static NullValue instance;

            NullValue();

            template <typename T>
            bool get(T& value) const;
    };

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
            ~ArbitraryValue();

            const Value::Type& getType() const;

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
            Value::Type type;

            union Data
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
            void create(Value::Type t);
            void create(Value::Type t, Data const& other);

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
