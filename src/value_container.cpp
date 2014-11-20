#include <value_container.h>
#include <QStringList>
#include <QDebug>

using namespace hfsmexec;

template<typename T>
struct ArbitraryValueTypeContainer;

template<>
struct ArbitraryValueTypeContainer<Undefined>
{
    static const ArbitraryValueType type = TYPE_UNDEFINED;
};

template<>
struct ArbitraryValueTypeContainer<Null>
{
    static const ArbitraryValueType type = TYPE_NULL;
};

template<>
struct ArbitraryValueTypeContainer<Boolean>
{
    static const ArbitraryValueType type = TYPE_BOOLEAN;
};

template<>
struct ArbitraryValueTypeContainer<Integer>
{
    static const ArbitraryValueType type = TYPE_INTEGER;
};

template<>
struct ArbitraryValueTypeContainer<Float>
{
    static const ArbitraryValueType type = TYPE_FLOAT;
};

template<>
struct ArbitraryValueTypeContainer<String>
{
    static const ArbitraryValueType type = TYPE_STRING;
};

template<>
struct ArbitraryValueTypeContainer<Array>
{
    static const ArbitraryValueType type = TYPE_ARRAY;
};

template<>
struct ArbitraryValueTypeContainer<Object>
{
    static const ArbitraryValueType type = TYPE_OBJECT;
};

/*
 * ArbitraryValueException
 */
ArbitraryValueException::ArbitraryValueException() :
    message("")
{

}

ArbitraryValueException::ArbitraryValueException(const QString& message):
    message(message)
{

}

ArbitraryValueException::~ArbitraryValueException() throw()
{

}

const char* ArbitraryValueException::what() const throw()
{
    return message.toStdString().c_str();
}

/*
 * ArbitraryValue
 */
ArbitraryValue::ArbitraryValue()
{
    create(TYPE_UNDEFINED);
}


ArbitraryValue::ArbitraryValue(ArbitraryValue const &other)
{
    create(other.type, other.data);
}

template<typename T>
ArbitraryValue::ArbitraryValue(T const &v)
{
    create<T>(v);
}

ArbitraryValue::~ArbitraryValue()
{
    destroy();
}

const ArbitraryValueType& ArbitraryValue::getType() const
{
    return type;
}

void* ArbitraryValue::ptr()
{
    return static_cast<void*>(&data);
}

void const* ArbitraryValue::ptr() const
{
    return static_cast<void const*>(&data);
}

template<typename T>
T& ArbitraryValue::get()
{
    ArbitraryValueType expected = ArbitraryValueTypeContainer<T>::type;
    if(expected != type)
    {
        throw ArbitraryValueException("invalid type");
    }

    switch(type)
    {
        case TYPE_UNDEFINED:
        case TYPE_NULL:
            throw ArbitraryValueException("non-fetchable type");
        default:
            return *static_cast<T*>(ptr());
    }
}

template<typename T>
T const& ArbitraryValue::get() const
{
    ArbitraryValueType expected = ArbitraryValueTypeContainer<T>::type;
    if(expected != type)
    {
        throw ArbitraryValueException("invalid type");
    }

    switch(type)
    {
        case TYPE_UNDEFINED:
        case TYPE_NULL:
            throw ArbitraryValueException("non-fetchable type");
        default:
            return *static_cast<T const*>(ptr());
    }
}

template<typename T>
void ArbitraryValue::set(T const& other)
{
    destroy();
    create<T>(other);
}

void ArbitraryValue::set(ArbitraryValue const& other)
{
    if(this != &other)
    {
        destroy();
        create(other.type, other.data);
    }
}

bool ArbitraryValue::operator==(ArbitraryValue const& other) const
{
    if(type != other.type)
    {
        return false;
    }

    switch(type)
    {
        case TYPE_BOOLEAN:
            return get<Boolean>() == other.get<Boolean>();
        case TYPE_INTEGER:
            return get<Integer>() == other.get<Integer>();
        case TYPE_FLOAT:
            return get<Float>() == other.get<Float>();
        case TYPE_STRING:
            return get<String>() == other.get<String>();
        case TYPE_OBJECT:
            return get<Object>() == other.get<Object>();
        case TYPE_ARRAY:
            return get<Array>() == other.get<Array>();
        default:
            return true;
    }
}

template<typename T>
void ArbitraryValue::create(T const &v)
{
    void* p = ptr();
    type = ArbitraryValueTypeContainer<T>::type;
    switch(type)
    {
        case TYPE_UNDEFINED:
        case TYPE_NULL:
        case TYPE_BOOLEAN:
        case TYPE_INTEGER:
        case TYPE_FLOAT:
            memcpy(&data, &v, sizeof(T));
            break;
        case TYPE_STRING:
            new(p) String(reinterpret_cast<String const&>(v));
            break;
        case TYPE_OBJECT:
            new(p) Object(reinterpret_cast<Object const&>(v));
            break;
        case TYPE_ARRAY:
            new(p) Array(reinterpret_cast<Array const&>(v));
            break;
    }
}

void ArbitraryValue::create(ArbitraryValueType t)
{
    type = t;
    memset(ptr(), 0, sizeof(data));
    switch(type)
    {
        case TYPE_UNDEFINED:
        case TYPE_NULL:
        case TYPE_BOOLEAN:
        case TYPE_INTEGER:
        case TYPE_FLOAT:
            break;
        case TYPE_STRING:
            new(ptr()) String();
            break;
        case TYPE_OBJECT:
            new(ptr()) Object();
            break;
        case TYPE_ARRAY:
            new(ptr()) Array();
            break;
    }
}

void ArbitraryValue::create(ArbitraryValueType t, DataUnion const& other)
{
    void* p = &data;
    type = t;
    switch(t)
    {
        case TYPE_UNDEFINED:
        case TYPE_NULL:
        case TYPE_BOOLEAN:
        case TYPE_INTEGER:
        case TYPE_FLOAT:
            memcpy(&data, &other, sizeof(data));
            break;
        case TYPE_STRING:
            new(p) String(reinterpret_cast<String const&>(other));
            break;
        case TYPE_OBJECT:
            new(p) Object(reinterpret_cast<Object const&>(other));
            break;
        case TYPE_ARRAY:
            new(p) Array(reinterpret_cast<Array const&>(other));
            break;
    }
}

void ArbitraryValue::destroy()
{
    switch(type)
    {
        case TYPE_UNDEFINED:
        case TYPE_NULL:
        case TYPE_BOOLEAN:
        case TYPE_INTEGER:
        case TYPE_FLOAT:
            break;
        case TYPE_STRING:
            static_cast<String*>(ptr())->~String();
            break;
        case TYPE_OBJECT:
            static_cast<Object*>(ptr())->~Object();
            break;
        case TYPE_ARRAY:
            static_cast<Array*>(ptr())->~Array();
            break;
    }

    memset(&data, 0, sizeof(data));
}

/*
 * ValueContainer
 */
ValueContainer::ValueContainer()
{

}

ValueContainer::ValueContainer(const ValueContainer& value) :
    value(value.value)
{

}

template<typename T>
ValueContainer::ValueContainer(const T& value)
{
    setValue<T>(value);
}

ValueContainer::~ValueContainer()
{

}

const ArbitraryValueType& ValueContainer::getType() const
{
    return value.getType();
}

bool ValueContainer::get(Boolean& value, Boolean defaultValue) const
{
    return getValue<Boolean>(value, defaultValue);
}

bool ValueContainer::get(Integer& value, Integer defaultValue) const
{
    return getValue<Integer>(value, defaultValue);
}

bool ValueContainer::get(Float& value, Float defaultValue) const
{
    return getValue<Float>(value, defaultValue);
}

bool ValueContainer::get(String& value, String defaultValue) const
{
    return getValue<String>(value, defaultValue);
}

bool ValueContainer::get(Array& value, Array defaultValue) const
{
    return getValue<Array>(value, defaultValue);
}

bool ValueContainer::get(Object& value, Object defaultValue) const
{
    return getValue<Object>(value, defaultValue);
}

bool ValueContainer::get(const QString& path, Boolean& value, Boolean defaultValue) const
{
    return getValue<Boolean>(path, value, defaultValue);
}

bool ValueContainer::get(const QString& path, Integer& value, Integer defaultValue) const
{
    return getValue<Integer>(path, value, defaultValue);
}

bool ValueContainer::get(const QString& path, Float& value, Float defaultValue) const
{
    return getValue<Float>(path, value, defaultValue);
}

bool ValueContainer::get(const QString& path, String& value, String defaultValue) const
{
    return getValue<String>(path, value, defaultValue);
}

bool ValueContainer::get(const QString& path, Array& value, Array defaultValue) const
{
    return getValue<Array>(path, value, defaultValue);
}

bool ValueContainer::get(const QString& path, Object& value, Object defaultValue) const
{
    return getValue<Object>(path, value, defaultValue);
}

bool ValueContainer::get(const QString& path, ValueContainer& value, ValueContainer defaultValue) const
{
    //TODO
}

void ValueContainer::set(const Boolean& value)
{
    setValue<Boolean>(value);
}

void ValueContainer::set(const Integer& value)
{
    setValue<Integer>(value);
}

void ValueContainer::set(const Float& value)
{
    setValue<Float>(value);
}

void ValueContainer::set(const String& value)
{
    setValue<String>(value);
}

void ValueContainer::set(const Array& value)
{
    setValue<Array>(value);
}

void ValueContainer::set(const Object& value)
{
    setValue<Object>(value);
}

void ValueContainer::set(const QString& path, const Boolean& value)
{
    setValue<Boolean>(path, value);
}

void ValueContainer::set(const QString& path, const Integer& value)
{
    setValue<Integer>(path, value);
}

void ValueContainer::set(const QString& path, const Float& value)
{
    setValue<Float>(path, value);
}

void ValueContainer::set(const QString& path, const String& value)
{
    setValue<String>(path, value);
}

void ValueContainer::set(const QString& path, const Array& value)
{
    setValue<Array>(path, value);
}

void ValueContainer::set(const QString& path, const Object& value)
{
    setValue<Object>(path, value);
}

void ValueContainer::set(const QString& path, const ValueContainer& value)
{
    //TODO
}

void ValueContainer::remove()
{
    //TODO
}

void ValueContainer::remove(const QString& path)
{
    //TODO
}

bool ValueContainer::toXml(const QString& path, QString& xml, XmlFormat format) const
{
    //TODO
}

bool ValueContainer::toJson(const QString& path, QString& json) const
{
    //TODO
}

bool ValueContainer::toYaml(const QString& path, QString& yaml) const
{
    //TODO
}

bool ValueContainer::fromXml(const QString& path, const QString& xml, XmlFormat format)
{
    //TODO
}

bool ValueContainer::fromJson(const QString& path, const QString& json)
{
    //TODO
}

bool ValueContainer::fromYaml(const QString& path, const QString& yaml)
{
    //TODO
}

const ValueContainer& ValueContainer::operator=(const ValueContainer& other)
{
    this->value.set(other.value);

    return *this;
}

bool ValueContainer::operator==(const ValueContainer& other) const
{
    return value == other.value;
}

ValueContainer& ValueContainer::operator[](const QString& name)
{
    if (getType() != TYPE_OBJECT)
    {
        set(Object());
    }

    Object& object = value.get<Object>();
    Object::iterator i = object.find(name);
    if (i == object.end())
    {
        qDebug() <<"create!";
        return object.insert(name, ValueContainer()).value();
    }

    return i.value();
}

const ValueContainer& ValueContainer::operator[](const QString& name) const
{
    if (getType() != TYPE_OBJECT)
    {
        throw ArbitraryValueException("value ist not of type object");
    }

    const Object& object = value.get<Object>();
    Object::const_iterator i = object.find(name);
    if (i == object.end())
    {
        throw ArbitraryValueException("member " + name + " not found");
    }

    return i.value();
}

ValueContainer& ValueContainer::operator[](int i)
{
    if (getType() != TYPE_ARRAY)
    {
        set(Array());
    }

    Array& array = value.get<Array>();
    for (int j = array.size() - i; j < 0; j++)
    {
        array.append(Null()); //TODO test
    }

    return array[i];
}

const ValueContainer& ValueContainer::operator[](int i) const
{
    if (getType() != TYPE_ARRAY)
    {
        throw ArbitraryValueException("value ist not of type array");
    }

    const Array& array = value.get<Array>();
    if (i >= array.size())
    {
        throw ArbitraryValueException("index out of bound");
    }

    return array[i];
}

bool ValueContainer::find(const QString& path, const ValueContainer*& value) const
{
    QString replacePath = path.trimmed().replace("[", "/[");
    QStringList splitPath = replacePath.split("/", QString::SkipEmptyParts);

    try
    {
        for (int i = 0; i < splitPath.size(); i++)
        {
            //access array value
            if (splitPath[i].at(0) == '[')
            {
                int index = splitPath[i].mid(1, 1).toInt();
                value = &(*value)[index];
            }
            //access other value
            else
            {
                QString name = splitPath[i];
                value = &(*value)[name];
            }
        }

        return true;
    }
    catch (ArbitraryValueException e)
    {
    }

    return false;
}

bool ValueContainer::find(const QString& path, ValueContainer*& value)
{
    QString replacePath = path.trimmed().replace("[", "/[");
    QStringList splitPath = replacePath.split("/", QString::SkipEmptyParts);

    for (int i = 0; i < splitPath.size(); i++)
    {
        //access array value
        if (splitPath[i].at(0) == '[')
        {
            int index = splitPath[i].mid(1, 1).toInt();
            value = &(*value)[index];
        }
        //access other value
        else
        {
            QString name = splitPath[i];
            value = &(*value)[name];
        }
    }

    return true;
}

template<typename T>
bool ValueContainer::getValue(T& value, T defaultValue) const
{
    try
    {
        value = this->value.get<T>();

        return true;
    }
    catch (ArbitraryValueException e)
    {
    }

    value = defaultValue;

    return false;
}

template<typename T>
bool ValueContainer::getValue(const QString& path, T& value, T defaultValue) const
{
    const ValueContainer* v = this;
    if (find(path, v))
    {
        return v->get(value, defaultValue);
    }

    return false;
}

template<typename T>
void ValueContainer::setValue(const T& value)
{
    this->value.set<T>(value);
}

template<typename T>
void ValueContainer::setValue(const QString& path, const T& value)
{
    ValueContainer* v = this;
    if (find(path, v))
    {
        v->set(value);
    }
}

/*
 * ValueContainerTest
 */
ValueContainerTest::ValueContainerTest()
{

}
