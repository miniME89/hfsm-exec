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

#include <parameter.h>

#include <QTextStream>
#include <QStringList>

#include <pugixml.hpp>
#include <jsoncpp/json/json.h>
#include <yaml-cpp/yaml.h>

using namespace hfsmexec;

template<typename T>
struct ArbitraryValueTypeContainer;

template<>
struct ArbitraryValueTypeContainer<Undefined>
{
    static const ValueType type = TYPE_UNDEFINED;
};

template<>
struct ArbitraryValueTypeContainer<Null>
{
    static const ValueType type = TYPE_NULL;
};

template<>
struct ArbitraryValueTypeContainer<Boolean>
{
    static const ValueType type = TYPE_BOOLEAN;
};

template<>
struct ArbitraryValueTypeContainer<Integer>
{
    static const ValueType type = TYPE_INTEGER;
};

template<>
struct ArbitraryValueTypeContainer<Float>
{
    static const ValueType type = TYPE_FLOAT;
};

template<>
struct ArbitraryValueTypeContainer<String>
{
    static const ValueType type = TYPE_STRING;
};

template<>
struct ArbitraryValueTypeContainer<Array>
{
    static const ValueType type = TYPE_ARRAY;
};

template<>
struct ArbitraryValueTypeContainer<Object>
{
    static const ValueType type = TYPE_OBJECT;
};

/*
 * ValueException
 */
ValueException::ValueException() :
    message("ValueException")
{

}

ValueException::ValueException(const QString& message):
    message(message)
{

}

ValueException::~ValueException() throw()
{

}

const char* ValueException::what() const throw()
{
    return message.toStdString().c_str();
}

/*
 * Value
 */
Value::Value() :
    refCounter(1)
{
    create(TYPE_UNDEFINED);
}


Value::Value(Value const &other) :
    refCounter(1)
{
    create(other.type, other.data);
}

template<typename T>
Value::Value(T const &v) :
    refCounter(1)
{
    create<T>(v);
}

Value::~Value()
{
    destroy();
}

const ValueType& Value::getType() const
{
    return type;
}

void* Value::ptr()
{
    return static_cast<void*>(&data);
}

void const* Value::ptr() const
{
    return static_cast<void const*>(&data);
}

template<typename T>
T& Value::get()
{
    ValueType expected = ArbitraryValueTypeContainer<T>::type;
    if(expected != type)
    {
        throw ValueException("invalid type");
    }

    switch(type)
    {
        case TYPE_UNDEFINED:
        case TYPE_NULL:
            throw ValueException("non-fetchable type");
        default:
            return *static_cast<T*>(ptr());
    }
}

template<typename T>
T const& Value::get() const
{
    ValueType expected = ArbitraryValueTypeContainer<T>::type;
    if(expected != type)
    {
        throw ValueException("invalid type");
    }

    switch(type)
    {
        case TYPE_UNDEFINED:
        case TYPE_NULL:
            throw ValueException("non-fetchable type");
        default:
            return *static_cast<T const*>(ptr());
    }
}

template<typename T>
void Value::set(T const& other)
{
    destroy();
    create<T>(other);
}

void Value::set(Value const& other)
{
    if(this != &other)
    {
        destroy();
        create(other.type, other.data);
    }
}

bool Value::operator==(Value const& other) const
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

void Value::incReference()
{
    mutexRefCounter.lock();
    refCounter++;
    mutexRefCounter.unlock();
}

void Value::decReference()
{
    mutexRefCounter.lock();
    refCounter--;
    mutexRefCounter.unlock();

    if (refCounter <= 0)
    {
        delete this;
    }
}

template<typename T>
void Value::create(T const &v)
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

void Value::create(ValueType t)
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

void Value::create(ValueType t, DataUnion const& other)
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

void Value::destroy()
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
 * Parameter
 */
const Logger* Parameter::logger = Logger::getLogger(LOGGER_VALUE);

Parameter::Parameter() :
    value(new Value())
{
    null();
}

Parameter::Parameter(const Boolean& value) :
    value(new Value())
{
    *this = value;
}

Parameter::Parameter(const Integer& value) :
    value(new Value())
{
    *this = value;
}

Parameter::Parameter(const Float& value) :
    value(new Value())
{
    *this = value;
}

Parameter::Parameter(const char* value) :
    value(new Value())
{
    *this = value;
}

Parameter::Parameter(const String& value) :
    value(new Value())
{
    *this = value;
}

Parameter::Parameter(const Array& value) :
    value(new Value())
{
    *this = value;
}

Parameter::Parameter(const Object& value) :
    value(new Value())
{
    *this = value;
}

Parameter::Parameter(const Parameter& value) :
    value(new Value())
{
    *this = value;
}

Parameter::Parameter(Parameter* const & value) :
    value(new Value())
{
    *this = value;
}

template<typename T>
Parameter::Parameter(const T& value)
{
    setValue<T>(value);
}

Parameter::~Parameter()
{
    value->decReference();
}

bool Parameter::isUndefined() const
{
    return getType() == TYPE_UNDEFINED;
}

bool Parameter::isNull() const
{
    return getType() == TYPE_NULL;
}

bool Parameter::isBoolean() const
{
    return getType() == TYPE_BOOLEAN;
}

bool Parameter::isInteger() const
{
    return getType() == TYPE_INTEGER;
}

bool Parameter::isFloat() const
{
    return getType() == TYPE_FLOAT;
}

bool Parameter::isString() const
{
    return getType() == TYPE_STRING;
}

bool Parameter::isArray() const
{
    return getType() == TYPE_ARRAY;
}

bool Parameter::isObject() const
{
    return getType() == TYPE_OBJECT;
}

Boolean Parameter::getBoolean(Boolean defaultValue) const
{
    Boolean value;
    get(value, defaultValue);

    return value;
}

Integer Parameter::getInteger(Integer defaultValue) const
{
    Integer value;
    get(value, defaultValue);

    return value;
}

Float Parameter::getFloat(Float defaultValue) const
{
    Float value;
    get(value, defaultValue);

    return value;
}

String Parameter::getString(String defaultValue) const
{
    String value;
    get(value, defaultValue);

    return value;
}

Array Parameter::getArray(Array defaultValue) const
{
    Array value;
    get(value, defaultValue);

    return value;
}

Object Parameter::getObject(Object defaultValue) const
{
    Object value;
    get(value, defaultValue);

    return value;
}

bool Parameter::get(Boolean& value, Boolean defaultValue) const
{
    return getValue<Boolean>(value, defaultValue);
}

bool Parameter::get(Integer& value, Integer defaultValue) const
{
    return getValue<Integer>(value, defaultValue);
}

bool Parameter::get(Float& value, Float defaultValue) const
{
    return getValue<Float>(value, defaultValue);
}

bool Parameter::get(String& value, String defaultValue) const
{
    return getValue<String>(value, defaultValue);
}

bool Parameter::get(Array& value, Array defaultValue) const
{
    return getValue<Array>(value, defaultValue);
}

bool Parameter::get(Object& value, Object defaultValue) const
{
    return getValue<Object>(value, defaultValue);
}

void Parameter::set(const Boolean& value)
{
    setValue<Boolean>(value);
}

void Parameter::set(const Integer& value)
{
    setValue<Integer>(value);
}

void Parameter::set(const Float& value)
{
    setValue<Float>(value);
}

void Parameter::set(const char* value)
{
    set(String(value));
}

void Parameter::set(const String& value)
{
    setValue<String>(value);
}

void Parameter::set(const Array& value)
{
    setValue<Array>(value);
}

void Parameter::set(const Object& value)
{
    setValue<Object>(value);
}

void Parameter::undefined()
{
    value->set<Undefined>(Undefined());
}

void Parameter::null()
{
    value->set<Null>(Null());
}

const ValueType& Parameter::getType() const
{
    return value->getType();
}

bool Parameter::toXml(QString& xml) const
{
    pugi::xml_document doc;
    if (!buildToXml(this, &doc))
    {
        logger->warning("couldn't build xml from value container");

        return false;
    }

    std::stringstream stream;
    pugi::xml_writer_stream writer(stream);
    doc.save(writer);

    xml = stream.str().c_str();

    return true;
}

bool Parameter::toJson(QString& json) const
{
    Json::Value root;
    if (!buildToJson(this, &root))
    {
        logger->warning("couldn't build json from value container");

        return false;
    }

    Json::StyledWriter writer;
    json = writer.write(root).c_str();

    return true;
}

bool Parameter::toYaml(QString& yaml) const
{
    YAML::Node root;
    if (!buildToYaml(this, &root))
    {
        logger->warning("couldn't build yaml from value container");

        return false;
    }

    YAML::Emitter writer;
    writer <<root;
    yaml = writer.c_str();

    return true;
}

bool Parameter::fromXml(const QString& xml)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_buffer(xml.toStdString().c_str(), xml.size());
    if (result.status != pugi::status_ok)
    {
        logger->warning("couldn't set value container from xml");

        return false;
    }

    pugi::xml_node root = doc.root();
    if (!buildFromXml(this, &root))
    {
        logger->warning("couldn't set value container from xml");

        return false;
    }

    return true;
}

bool Parameter::fromJson(const QString& json)
{
    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(json.toStdString(), root))
    {
        logger->warning(QString("couldn't set value container from json: %1").arg(reader.getFormatedErrorMessages().c_str()));

        return false;
    }

    if (!buildFromJson(this, &root))
    {
        logger->warning("couldn't set value container from json");

        return false;
    }

    return true;
}

bool Parameter::fromYaml(const QString& yaml)
{
    try
    {
        YAML::Node root = YAML::Load(yaml.toStdString());

        if (!buildFromYaml(this, &root))
        {
            logger->warning("couldn't set value container from yaml");

            return false;
        }

        return true;
    }
    catch (YAML::Exception e)
    {
        logger->warning(QString("couldn't set value container from yaml: %1").arg(e.msg.c_str()));
    }

    return false;
}

const Parameter& Parameter::operator=(const Boolean& value)
{
    set(value);

    return *this;
}

const Parameter& Parameter::operator=(const Integer& value)
{
    set(value);

    return *this;
}

const Parameter& Parameter::operator=(const Float& value)
{
    set(value);

    return *this;
}

const Parameter& Parameter::operator=(const char* value)
{
    set(value);

    return *this;
}

const Parameter& Parameter::operator=(const String& value)
{
    set(value);

    return *this;
}

const Parameter& Parameter::operator=(const Array& value)
{
    set(value);

    return *this;
}

const Parameter& Parameter::operator=(const Object& value)
{
    set(value);

    return *this;
}

const Parameter& Parameter::operator=(const Parameter& other)
{
    this->value->set(*other.value);

    return *this;
}

const Parameter& Parameter::operator=(const Parameter* other)
{
    value->decReference();
    value = other->value;
    value->incReference();

    return *this;
}

bool Parameter::operator==(const Parameter& other) const
{
    return value == other.value;
}

Parameter& Parameter::operator[](const QString& path)
{
    QStringList splitPath = path.trimmed().split("/", QString::SkipEmptyParts);

    Parameter* value = this;
    for (int i = 0; i < splitPath.size(); i++)
    {
        QString name = splitPath[i];

        if (value->getType() != TYPE_OBJECT)
        {
            value->set(Object());
        }

        Object& object = value->value->get<Object>();
        Object::iterator it = object.find(name.toStdString().c_str());
        //value does not exist
        if (it == object.end())
        {
            value = &object.insert(name.toStdString().c_str(), Parameter()).value();
            value->null();
        }
        //value already exist
        else
        {
            value = &it.value();
        }
    }

    return *value;
}

const Parameter& Parameter::operator[](const QString& path) const
{
    QStringList splitPath = path.trimmed().split("/", QString::SkipEmptyParts);

    const Parameter* value = this;
    for (int i = 0; i < splitPath.size(); i++)
    {
        QString name = splitPath[i];

        if (value->getType() != TYPE_OBJECT)
        {
            throw ValueException("value ist not of type object");
        }

        const Object& object = value->value->get<Object>();
        Object::const_iterator it = object.find(name.toStdString().c_str());
        //value does not exist
        if (it == object.end())
        {
            throw ValueException("member " + name + " not found");
        }
        //value already exist
        else
        {
            value = &it.value();
        }
    }

    return *value;
}


Parameter& Parameter::operator[](int i)
{
    if (getType() != TYPE_ARRAY)
    {
        set(Array());
    }

    Array& array = value->get<Array>();
    for (int j = array.size() - i - 1; j < 0; j++)
    {
        array.append(Null());
    }

    return array[i];
}

const Parameter& Parameter::operator[](int i) const
{
    if (getType() != TYPE_ARRAY)
    {
        throw ValueException("value ist not of type array");
    }

    const Array& array = value->get<Array>();
    if (i >= array.size())
    {
        throw ValueException("index out of bound");
    }

    return array[i];
}

template<typename T>
bool Parameter::getValue(T& value, T defaultValue) const
{
    try
    {
        value = this->value->get<T>();

        return true;
    }
    catch (ValueException e)
    {
    }

    value = defaultValue;

    return false;
}

template<typename T>
void Parameter::setValue(const T& value)
{
    this->value->set<T>(value);
}

bool Parameter::buildToXml(const Parameter* value, void* data) const
{
    pugi::xml_node* xmlValue = static_cast<pugi::xml_node*>(data);

    if (value->isUndefined())
    {
        return false;
    }
    else if (value->isNull())
    {

    }
    else if (value->isBoolean())
    {
        Boolean v;
        value->get(v);
        xmlValue->text().set(v);
    }
    else if (value->isInteger())
    {
        Integer v;
        value->get(v);
        xmlValue->text().set(QString::number(v).toStdString().c_str());
    }
    else if (value->isFloat())
    {
        Float v;
        value->get(v);
        xmlValue->text().set(QString::number(v).toStdString().c_str());
    }
    else if (value->isString())
    {
        String v;
        value->get(v);
        xmlValue->text().set(v.toStdString().c_str());
    }
    else if (value->isArray())
    {
        Array v;
        value->get(v);
        for (int i = 0; i < v.size(); i++)
        {
            pugi::xml_node dataChild = xmlValue->append_child("item");
            if (!buildToXml(&v[i], &dataChild))
            {
                return false;
            }
        }
    }
    else if (value->isObject())
    {
        Object v;
        value->get(v);
        for (Object::const_iterator it = v.begin(); it != v.end(); it++)
        {
            pugi::xml_node dataChild = xmlValue->append_child(it.key().toStdString().c_str());
            if (!buildToXml(&it.value(), &dataChild))
            {
                return false;
            }
        }
    }

    return true;
}

bool Parameter::buildToJson(const Parameter* value, void* data) const
{
    Json::Value* jsonValue = static_cast<Json::Value*>(data);

    if (value->isUndefined())
    {
        return false;
    }
    else if (value->isNull())
    {

    }
    else if (value->isBoolean())
    {
        Boolean v;
        value->get(v);
        *jsonValue = v;
    }
    else if (value->isInteger())
    {
        Integer v;
        value->get(v);
        *jsonValue = v;
    }
    else if (value->isFloat())
    {
        Float v;
        value->get(v);
        *jsonValue = v;
    }
    else if (value->isString())
    {
        String v;
        value->get(v);
        *jsonValue = v.toStdString();
    }
    else if (value->isArray())
    {
        Array v;
        value->get(v);
        *jsonValue = Json::Value(Json::arrayValue);
        for (int i = 0; i < v.size(); i++)
        {
            Json::Value dataChild;
            if (!buildToJson(&v[i], &dataChild))
            {
                return false;
            }
            (*jsonValue)[i] = dataChild;
        }
    }
    else if (value->isObject())
    {
        Object v;
        value->get(v);
        *jsonValue = Json::Value(Json::objectValue);
        for (Object::const_iterator it = v.begin(); it != v.end(); it++)
        {
            Json::Value dataChild;
            if (!buildToJson(&it.value(), &dataChild))
            {
                return false;
            }
            (*jsonValue)[it.key().toStdString()] = dataChild;
        }
    }

    return true;
}

bool Parameter::buildToYaml(const Parameter* value, void* data) const
{
    YAML::Node* yamlValue = static_cast<YAML::Node*>(data);

    if (value->isUndefined())
    {
        return false;
    }
    else if (value->isNull())
    {

    }
    else if (value->isBoolean())
    {
        Boolean v;
        value->get(v);
        *yamlValue = v;
    }
    else if (value->isInteger())
    {
        Integer v;
        value->get(v);
        *yamlValue = v;
    }
    else if (value->isFloat())
    {
        Float v;
        value->get(v);
        *yamlValue = v;
    }
    else if (value->isString())
    {
        String v;
        value->get(v);
        *yamlValue = v.toStdString();
    }
    else if (value->isArray())
    {
        Array v;
        value->get(v);
        for (int i = 0; i < v.size(); i++)
        {
            YAML::Node dataChild;
            if (!buildToYaml(&v[i], &dataChild))
            {
                return false;
            }
            yamlValue->push_back(dataChild);
        }
    }
    else if (value->isObject())
    {
        Object v;
        value->get(v);
        for (Object::const_iterator it = v.begin(); it != v.end(); it++)
        {
            YAML::Node dataChild;
            if (!buildToYaml(&it.value(), &dataChild))
            {
                return false;
            }
            (*yamlValue)[it.key().toStdString()] = dataChild;
        }
    }

    return true;
}

bool Parameter::buildFromXml(Parameter* value, void* data)
{
    pugi::xml_node* xmlValue = static_cast<pugi::xml_node*>(data);
    pugi::xml_attribute typeAttribute = xmlValue->attribute("type");
    pugi::xml_text textContent = xmlValue->text();

    QString type = typeAttribute.value();

    if (type == "Boolean")
    {
        value->set(textContent.as_bool());
    }
    else if (type == "Integer")
    {
        value->set(textContent.as_int());
    }
    else if (type == "Float")
    {
        value->set(textContent.as_double());
    }
    else if (type == "String")
    {
        value->set(textContent.as_string());
    }
    else if (type == "Array")
    {
        Array array;
        for (pugi::xml_node_iterator i = xmlValue->begin(); i != xmlValue->end(); i++)
        {
            Parameter v;
            if (!buildFromXml(&v, &i))
            {
                return false;
            }
            array.append(v);
        }
        value->set(array);
    }
    else
    {
        Object object;
        for (pugi::xml_node_iterator i = xmlValue->begin(); i != xmlValue->end(); i++)
        {
            Parameter v;
            if (!buildFromXml(&v, &i))
            {
                return false;
            }

            pugi::xml_attribute nameAttribute = i->attribute("name");
            QString key = i->name();

            if (!nameAttribute.empty())
            {
                key = nameAttribute.value();
            }

            object.insert(key, v);
        }
        value->set(object);
    }

    return true;
}

bool Parameter::buildFromJson(Parameter* value, void* data)
{
    Json::Value* jsonValue = static_cast<Json::Value*>(data);

    if (jsonValue->isNull())
    {
        value->null();
    }
    else if (jsonValue->isBool())
    {
        value->set(jsonValue->asBool());
    }
    else if (jsonValue->isInt() || jsonValue->isUInt())
    {
        value->set(jsonValue->asInt());
    }
    else if (jsonValue->isDouble())
    {
        value->set(jsonValue->asDouble());
    }
    else if (jsonValue->isString())
    {
        value->set(jsonValue->asString().c_str());
    }
    else if (jsonValue->isArray())
    {
        Array array;
        for (unsigned int i = 0; i < jsonValue->size(); i++)
        {
            Parameter v;
            if (!buildFromJson(&v, &jsonValue[i]))
            {
                return false;
            }
            array.append(v);
        }
        value->set(array);
    }
    else if (jsonValue->isObject())
    {
        Object object;
        for (Json::ValueIterator i = jsonValue->begin(); i != jsonValue->end(); i++)
        {
            Parameter v;
            if (!buildFromJson(&v, &*i))
            {
                return false;
            }
            object.insert(i.memberName(), v);
        }
        value->set(object);
    }

    return true;
}

bool Parameter::buildFromYaml(Parameter* value, void* data)
{
    YAML::Node* yamlValue = static_cast<YAML::Node*>(data);

    if (yamlValue->IsNull())
    {
        value->null();
    }
    else if (yamlValue->IsScalar())
    {
        try
        {
            value->set(yamlValue->as<Boolean>());
        }
        catch (const YAML::BadConversion& e)
        {
            try
            {
                value->set(yamlValue->as<Integer>());
            }
            catch (const YAML::BadConversion& e)
            {
                try
                {
                    value->set(yamlValue->as<Float>());
                }
                catch (const YAML::BadConversion& e)
                {
                    std::string s = yamlValue->as<std::string>();
                    value->set(s.c_str());
                }
            }
        }
    }
    else if (yamlValue->IsSequence())
    {
        Array array;
        for (unsigned int i = 0; i < yamlValue->size(); i++)
        {
            Parameter v;
            YAML::Node n = (*yamlValue)[i];
            if (!buildFromYaml(&v, &n))
            {
                return false;
            }
            array.append(v);
        }
        value->set(array);
    }
    else if (yamlValue->IsMap())
    {
        Object object;
        for (YAML::iterator i = yamlValue->begin(); i != yamlValue->end(); i++)
        {
            Parameter v;
            if (!buildFromYaml(&v, &i->second))
            {
                return false;
            }
            object.insert(i->first.as<std::string>().c_str(), v);
        }
        value->set(object);
    }

    return true;
}
