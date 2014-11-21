#ifndef PARAMETERCONTAINER_H
#define PARAMETERCONTAINER_H

#include <QString>
#include <QList>
#include <QMap>
#include <exception>

namespace hfsmexec
{
    class ValueContainer;

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
    typedef QList<ValueContainer> Array;
    typedef QMap<String, ValueContainer> Object;

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
            T &get();
            template<typename T>
            T const &get() const;

            template<typename T>
            void set(T const& other);
            void set(ArbitraryValue const &other);

            bool operator==(ArbitraryValue const &other) const;

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

    class ValueContainer
    {
        public:
            enum XmlFormat
            {
                NAME_TAG,
                PARAMETER_TAG
            };

            ValueContainer();
            ValueContainer(const ValueContainer& value);
            template<typename T>
            ValueContainer(const T& value);
            ~ValueContainer();

            const ArbitraryValueType& getType() const;

            void undefined();
            void null();

            bool get(Boolean& value, Boolean defaultValue = false) const;
            bool get(Integer& value, Integer defaultValue = 0) const;
            bool get(Float& value, Float defaultValue = 0) const;
            bool get(String& value, String defaultValue = "") const;
            bool get(Array& value, Array defaultValue = Array()) const;
            bool get(Object& value, Object defaultValue = Object()) const;

            bool get(const QString& path, Boolean& value, Boolean defaultValue = false) const;
            bool get(const QString& path, Integer& value, Integer defaultValue = 0) const;
            bool get(const QString& path, Float& value, Float defaultValue = 0.0) const;
            bool get(const QString& path, String& value, String defaultValue = "") const;
            bool get(const QString& path, Array& value, Array defaultValue = Array()) const;
            bool get(const QString& path, Object& value, Object defaultValue = Object()) const;
            bool get(const QString& path, ValueContainer& value, ValueContainer defaultValue = ValueContainer()) const;

            void set(const Boolean& value);
            void set(const Integer& value);
            void set(const Float& value);
            void set(const char* value);
            void set(const String& value);
            void set(const Array& value);
            void set(const Object& value);
            void set(const ValueContainer& value);

            void set(const QString& path, const Boolean& value);
            void set(const QString& path, const Integer& value);
            void set(const QString& path, const Float& value);
            void set(const QString& path, const char* value);
            void set(const QString& path, const String& value);
            void set(const QString& path, const Array& value);
            void set(const QString& path, const Object& value);
            void set(const QString& path, const ValueContainer& value);

            void remove(const QString& path);

            bool isUndefined() const;
            bool isNull() const;
            bool isBoolean() const;
            bool isInteger() const;
            bool isFloat() const;
            bool isString() const;
            bool isArray() const;
            bool isObject() const;

            bool toXml(QString& xml) const;
            bool toJson(QString& json) const;
            bool toYaml(QString& yaml) const;

            bool fromXml(const QString& xml);
            bool fromJson(const QString& json);
            bool fromYaml(const QString& yaml);

            const ValueContainer& operator=(const ValueContainer& other);
            bool operator==(const ValueContainer& other) const;
            ValueContainer& operator[](const QString& name);
            const ValueContainer& operator[](const QString& name) const;
            ValueContainer& operator[](int i);
            const ValueContainer& operator[](int i) const;

        private:
            ArbitraryValue value;

            template <typename T>
            bool getValue(T& value, T defaultValue) const;

            template <typename T>
            bool getValue(const QString& path, T& value, T defaultValue) const;

            template <typename T>
            void setValue(const T& value);

            template <typename T>
            void setValue(const QString& path, const T& value);

            bool findValue(const QString& path, const ValueContainer*& value) const;
            bool findValue(const QString& path, ValueContainer*& value);

            bool buildToXml(const ValueContainer* value, void* data) const;
            bool buildToJson(const ValueContainer* value, void* data) const;
            bool buildToYaml(const ValueContainer* value, void* data) const;

            bool buildFromXml(ValueContainer* value, void* data);
            bool buildFromJson(ValueContainer* value, void* data);
            bool buildFromYaml(ValueContainer* value, void* data);
    };

    class ValueContainerTest
    {
        public:
            ValueContainerTest();
    };
}
#endif
