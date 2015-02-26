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

#include <value.h>

#include <pugixml.hpp>
#include <json/json.h>
#include <yaml-cpp/yaml.h>

using namespace hfsmexec;

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
const Logger* Value::logger = Logger::getLogger(LOGGER_VALUE);

Value::Value() :
    value(new ArbitraryValue())
{
    null();
}

Value::Value(const Boolean& value) :
    value(new ArbitraryValue())
{
    *this = value;
}

Value::Value(const Integer& value) :
    value(new ArbitraryValue())
{
    *this = value;
}

Value::Value(const Float& value) :
    value(new ArbitraryValue())
{
    *this = value;
}

Value::Value(const char* value) :
    value(new ArbitraryValue())
{
    *this = value;
}

Value::Value(const String& value) :
    value(new ArbitraryValue())
{
    *this = value;
}

Value::Value(const Array& value) :
    value(new ArbitraryValue())
{
    *this = value;
}

Value::Value(const Object& value) :
    value(new ArbitraryValue())
{
    *this = value;
}

Value::Value(const Value& value) :
    value(new ArbitraryValue())
{
    *this = value;
}

Value::Value(Value* const & value) :
    value(new ArbitraryValue())
{
    *this = value;
}

template<typename T>
Value::Value(const T& value)
{
    setValue<T>(value);
}

Value::~Value()
{
    value->decReference();
}

bool Value::isUndefined() const
{
    return getType() == TYPE_UNDEFINED;
}

bool Value::isNull() const
{
    return getType() == TYPE_NULL;
}

bool Value::isBoolean() const
{
    return getType() == TYPE_BOOLEAN;
}

bool Value::isInteger() const
{
    return getType() == TYPE_INTEGER;
}

bool Value::isFloat() const
{
    return getType() == TYPE_FLOAT;
}

bool Value::isString() const
{
    return getType() == TYPE_STRING;
}

bool Value::isArray() const
{
    return getType() == TYPE_ARRAY;
}

bool Value::isObject() const
{
    return getType() == TYPE_OBJECT;
}

Value::Boolean Value::getBoolean(Boolean defaultValue) const
{
    Boolean value;
    get(value, defaultValue);

    return value;
}

Value::Integer Value::getInteger(Integer defaultValue) const
{
    Integer value;
    get(value, defaultValue);

    return value;
}

Value::Float Value::getFloat(Float defaultValue) const
{
    Float value;
    get(value, defaultValue);

    return value;
}

Value::String Value::getString(String defaultValue) const
{
    String value;
    get(value, defaultValue);

    return value;
}

Value::Array Value::getArray(Array defaultValue) const
{
    Array value;
    get(value, defaultValue);

    return value;
}

Value::Object Value::getObject(Object defaultValue) const
{
    Object value;
    get(value, defaultValue);

    return value;
}

bool Value::get(Boolean& value, Boolean defaultValue) const
{
    return getValue<Boolean>(value, defaultValue);
}

bool Value::get(Integer& value, Integer defaultValue) const
{
    return getValue<Integer>(value, defaultValue);
}

bool Value::get(Float& value, Float defaultValue) const
{
    return getValue<Float>(value, defaultValue);
}

bool Value::get(String& value, String defaultValue) const
{
    return getValue<String>(value, defaultValue);
}

bool Value::get(Array& value, Array defaultValue) const
{
    return getValue<Array>(value, defaultValue);
}

bool Value::get(Object& value, Object defaultValue) const
{
    return getValue<Object>(value, defaultValue);
}

void Value::set(const Boolean& value)
{
    setValue<Boolean>(value);
}

void Value::set(const Integer& value)
{
    setValue<Integer>(value);
}

void Value::set(const Float& value)
{
    setValue<Float>(value);
}

void Value::set(const char* value)
{
    set(String(value));
}

void Value::set(const String& value)
{
    setValue<String>(value);
}

void Value::set(const Array& value)
{
    setValue<Array>(value);
}

void Value::set(const Object& value)
{
    setValue<Object>(value);
}

Value& Value::getValue(const QString& path)
{
    QStringList splitPath = path.trimmed().split(QRegExp("(\\.|\\[)"), QString::SkipEmptyParts);

    Value* value = this;
    for (int i = 0; i < splitPath.size(); i++)
    {
        QString name = splitPath[i];
        if (name.at(name.length() - 1) == ']')
        {
            int index = name.remove(name.length() - 1, 1).toInt();
            value = &(*value)[index];
        }
        else
        {
            value = &(*value)[name];
        }
    }

    return *value;
}

const Value& Value::getValue(const QString& path) const
{
    QStringList splitPath = path.trimmed().split(QRegExp("(\\.|\\[)"), QString::SkipEmptyParts);

    const Value* value = this;
    for (int i = 0; i < splitPath.size(); i++)
    {
        QString name = splitPath[i];
        if (name.at(name.length() - 1) == ']')
        {
            int index = name.remove(name.length() - 1, 1).toInt();
            value = &(*value)[index];
        }
        else
        {
            value = &(*value)[name];
        }
    }

    return *value;
}

int Value::size()
{
    if (value->getType() == TYPE_ARRAY)
    {
        Array& array = value->get<Array>();

        return array.size();
    }

    return -1;
}

void Value::remove(const QString& key)
{
    if (value->getType() == TYPE_OBJECT)
    {
        Object& object = value->get<Object>();
        object.remove(key);
    }
}

void Value::remove(int i)
{
    if (value->getType() == TYPE_ARRAY)
    {
        Array& array = value->get<Array>();
        array.removeAt(i);
    }
}

bool Value::contains(const QString& key)
{
    if (value->getType() == TYPE_OBJECT)
    {
        Object& object = value->get<Object>();
        Object::iterator it = object.find(key.toStdString().c_str());

        return it != object.end();
    }

    return false;
}

void Value::undefined()
{
    value->set<Undefined>(Undefined());
}

void Value::null()
{
    value->set<Null>(Null());
}

const Value::ValueType& Value::getType() const
{
    return value->getType();
}

Value::String Value::toString()
{
    if (value->getType() == TYPE_UNDEFINED)
    {
        return "undefined";
    }
    else if (value->getType() == TYPE_NULL)
    {
        return "null";
    }
    else if (value->getType() == TYPE_BOOLEAN)
    {
        Boolean& v = value->get<Boolean>();

        return (v) ? "true" : "false";
    }
    else if (value->getType() == TYPE_INTEGER)
    {
        Integer& v = value->get<Integer>();

        return QString::number(v);
    }
    else if (value->getType() == TYPE_FLOAT)
    {
        Float& v = value->get<Float>();

        return QString::number(v);
    }
    else if (value->getType() == TYPE_STRING)
    {
        String& v = value->get<String>();

        return String(v);
    }
    else if (value->getType() == TYPE_ARRAY)
    {
        return "[Array]";
    }
    else if (value->getType() == TYPE_OBJECT)
    {
        return "[Object]";
    }
}

bool Value::toXml(QString& xml) const
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

bool Value::toJson(QString& json) const
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

bool Value::toYaml(QString& yaml) const
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

bool Value::fromXml(const QString& xml)
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

bool Value::fromJson(const QString& json)
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

bool Value::fromYaml(const QString& yaml)
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

const Value& Value::operator=(const Boolean& value)
{
    set(value);

    return *this;
}

const Value& Value::operator=(const Integer& value)
{
    set(value);

    return *this;
}

const Value& Value::operator=(const Float& value)
{
    set(value);

    return *this;
}

const Value& Value::operator=(const char* value)
{
    set(value);

    return *this;
}

const Value& Value::operator=(const String& value)
{
    set(value);

    return *this;
}

const Value& Value::operator=(const Array& value)
{
    set(value);

    return *this;
}

const Value& Value::operator=(const Object& value)
{
    set(value);

    return *this;
}

const Value& Value::operator=(const Value& other)
{
    this->value->set(*other.value);

    return *this;
}

const Value& Value::operator=(const Value* other)
{
    value->decReference();
    value = other->value;
    value->incReference();

    return *this;
}

bool Value::operator==(const Value& other) const
{
    return *value == *other.value;
}

bool Value::operator!=(const Value& other) const
{
    return !(*value == *other.value);
}

Value& Value::operator[](const QString& name)
{
    if (getType() != TYPE_OBJECT)
    {
        value->set(Object());
    }

    Object& object = value->get<Object>();
    Object::iterator it = object.find(name);
    //value does not exist
    if (it == object.end())
    {
        Value& value = object.insert(name, Value()).value();
        value.null();

        return value;
    }
    //value exist
    else
    {
        return it.value();
    }
}

const Value& Value::operator[](const QString& name) const
{
    if (getType() != TYPE_OBJECT)
    {
        throw ValueException("value ist not of type object");
    }

    const Object& object = value->get<Object>();
    Object::const_iterator it = object.find(name);
    //value does not exist
    if (it == object.end())
    {
        throw ValueException("member " + name + " not found");
    }
    //value exist
    else
    {
        return it.value();
    }
}

Value& Value::operator[](int i)
{
    if (getType() != TYPE_ARRAY)
    {
        set(Array());
    }

    Array& array = value->get<Array>();
    for (int j = array.size() - i - 1; j < 0; j++)
    {
        array.append(Value());
    }

    return array[i];
}

const Value& Value::operator[](int i) const
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
bool Value::getValue(T& value, T defaultValue) const
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
void Value::setValue(const T& value)
{
    this->value->set<T>(value);
}

bool Value::buildToXml(const Value* value, pugi::xml_node* xmlValue) const
{
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

bool Value::buildToJson(const Value* value, Json::Value* jsonValue) const
{
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

bool Value::buildToYaml(const Value* value, YAML::Node* yamlValue) const
{
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

bool Value::buildFromXml(Value* value, pugi::xml_node* xmlValue)
{
    pugi::xml_attribute typeAttribute = xmlValue->attribute("type");
    pugi::xml_text textContent = xmlValue->text();

    QString type = typeAttribute.value();

    if (type == "Boolean")
    {
        value->set(textContent.as_bool());
    }
    else if (type == "Integer")
    {
        value->set(QString(textContent.get()).toInt());
    }
    else if (type == "Float")
    {
        value->set(QString(textContent.get()).toDouble());
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
            Value v;
            if (!buildFromXml(&v, (pugi::xml_node*)&i))
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
            Value v;
            if (!buildFromXml(&v, (pugi::xml_node*)&i))
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

bool Value::buildFromJson(Value* value, Json::Value* jsonValue)
{
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
            Value v;
            if (!buildFromJson(&v, &(*jsonValue)[i]))
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
            Value v;
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

bool Value::buildFromYaml(Value* value, YAML::Node* yamlValue)
{
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
            Value v;
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
            Value v;
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

/*
 * ArbitraryValueTypeContainer
 */
template<typename T>
struct ArbitraryValueTypeContainer;

template<>
struct ArbitraryValueTypeContainer<Value::Undefined>
{
    static const Value::ValueType type = Value::TYPE_UNDEFINED;
};

template<>
struct ArbitraryValueTypeContainer<Value::Null>
{
    static const Value::ValueType type = Value::TYPE_NULL;
};

template<>
struct ArbitraryValueTypeContainer<Value::Boolean>
{
    static const Value::ValueType type = Value::TYPE_BOOLEAN;
};

template<>
struct ArbitraryValueTypeContainer<Value::Integer>
{
    static const Value::ValueType type = Value::TYPE_INTEGER;
};

template<>
struct ArbitraryValueTypeContainer<Value::Float>
{
    static const Value::ValueType type = Value::TYPE_FLOAT;
};

template<>
struct ArbitraryValueTypeContainer<Value::String>
{
    static const Value::ValueType type = Value::TYPE_STRING;
};

template<>
struct ArbitraryValueTypeContainer<Value::Array>
{
    static const Value::ValueType type = Value::TYPE_ARRAY;
};

template<>
struct ArbitraryValueTypeContainer<Value::Object>
{
    static const Value::ValueType type = Value::TYPE_OBJECT;
};

/*
 * ArbitraryValue
 */
ArbitraryValue::ArbitraryValue() :
    refCounter(1)
{
    create(Value::TYPE_UNDEFINED);
}


ArbitraryValue::ArbitraryValue(ArbitraryValue const &other) :
    refCounter(1)
{
    create(other.type, other.data);
}

template<typename T>
ArbitraryValue::ArbitraryValue(T const &v) :
    refCounter(1)
{
    create<T>(v);
}

ArbitraryValue::~ArbitraryValue()
{
    destroy();
}

const Value::ValueType& ArbitraryValue::getType() const
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
    Value::ValueType expected = ArbitraryValueTypeContainer<T>::type;
    if(expected != type)
    {
        throw ValueException("invalid type");
    }

    switch(type)
    {
        case Value::TYPE_UNDEFINED:
        case Value::TYPE_NULL:
            throw ValueException("non-fetchable type");
        default:
            return *static_cast<T*>(ptr());
    }
}

template<typename T>
T const& ArbitraryValue::get() const
{
    Value::ValueType expected = ArbitraryValueTypeContainer<T>::type;
    if(expected != type)
    {
        throw ValueException("invalid type");
    }

    switch(type)
    {
        case Value::TYPE_UNDEFINED:
        case Value::TYPE_NULL:
            throw ValueException("non-fetchable type");
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
        case Value::TYPE_BOOLEAN:
            return get<Value::Boolean>() == other.get<Value::Boolean>();
        case Value::TYPE_INTEGER:
            return get<Value::Integer>() == other.get<Value::Integer>();
        case Value::TYPE_FLOAT:
            return get<Value::Float>() == other.get<Value::Float>();
        case Value::TYPE_STRING:
            return get<Value::String>() == other.get<Value::String>();
        case Value::TYPE_OBJECT:
            return get<Value::Object>() == other.get<Value::Object>();
        case Value::TYPE_ARRAY:
            return get<Value::Array>() == other.get<Value::Array>();
        default:
            return false;
    }
}

void ArbitraryValue::incReference()
{
    mutexRefCounter.lock();
    refCounter++;
    mutexRefCounter.unlock();
}

void ArbitraryValue::decReference()
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
void ArbitraryValue::create(T const &v)
{
    void* p = ptr();
    type = ArbitraryValueTypeContainer<T>::type;
    switch(type)
    {
        case Value::TYPE_UNDEFINED:
        case Value::TYPE_NULL:
        case Value::TYPE_BOOLEAN:
        case Value::TYPE_INTEGER:
        case Value::TYPE_FLOAT:
            memcpy(&data, &v, sizeof(T));
            break;
        case Value::TYPE_STRING:
            new(p) Value::String(reinterpret_cast<Value::String const&>(v));
            break;
        case Value::TYPE_OBJECT:
            new(p) Value::Object(reinterpret_cast<Value::Object const&>(v));
            break;
        case Value::TYPE_ARRAY:
            new(p) Value::Array(reinterpret_cast<Value::Array const&>(v));
            break;
    }
}

void ArbitraryValue::create(Value::ValueType t)
{
    type = t;
    memset(ptr(), 0, sizeof(data));
    switch(type)
    {
        case Value::TYPE_UNDEFINED:
        case Value::TYPE_NULL:
        case Value::TYPE_BOOLEAN:
        case Value::TYPE_INTEGER:
        case Value::TYPE_FLOAT:
            break;
        case Value::TYPE_STRING:
            new(ptr()) Value::String();
            break;
        case Value::TYPE_OBJECT:
            new(ptr()) Value::Object();
            break;
        case Value::TYPE_ARRAY:
            new(ptr()) Value::Array();
            break;
    }
}

void ArbitraryValue::create(Value::ValueType t, DataUnion const& other)
{
    void* p = &data;
    type = t;
    switch(t)
    {
        case Value::TYPE_UNDEFINED:
        case Value::TYPE_NULL:
        case Value::TYPE_BOOLEAN:
        case Value::TYPE_INTEGER:
        case Value::TYPE_FLOAT:
            memcpy(&data, &other, sizeof(data));
            break;
        case Value::TYPE_STRING:
            new(p) Value::String(reinterpret_cast<Value::String const&>(other));
            break;
        case Value::TYPE_OBJECT:
            new(p) Value::Object(reinterpret_cast<Value::Object const&>(other));
            break;
        case Value::TYPE_ARRAY:
            new(p) Value::Array(reinterpret_cast<Value::Array const&>(other));
            break;
    }
}

void ArbitraryValue::destroy()
{
    typedef Value::String String;
    typedef Value::Object Object;
    typedef Value::Array Array;

    switch(type)
    {
        case Value::TYPE_UNDEFINED:
        case Value::TYPE_NULL:
        case Value::TYPE_BOOLEAN:
        case Value::TYPE_INTEGER:
        case Value::TYPE_FLOAT:
            break;
        case Value::TYPE_STRING:
            static_cast<Value::String*>(ptr())->~String();
            break;
        case Value::TYPE_OBJECT:
            static_cast<Value::Object*>(ptr())->~Object();
            break;
        case Value::TYPE_ARRAY:
            static_cast<Value::Array*>(ptr())->~Array();
            break;
    }

    memset(&data, 0, sizeof(data));
}

/*
 * ValueScriptBinding
 */
ValueScriptBinding::ValueScriptBinding(QScriptEngine* engine) :
    QScriptClass(engine)
{

}

ValueScriptBinding::~ValueScriptBinding()
{

}

QScriptValue ValueScriptBinding::property(const QScriptValue& object, const QScriptString& name, uint id)
{
    Value* value = qscriptvalue_cast<Value*>(object.data());
    if (!value)
    {
        return QScriptValue();
    }

    //get value from array or object
    if (value->isArray() && name.toArrayIndex() < value->size())
    {
        value = &((*value)[name.toArrayIndex()]);
    }
    else if (value->isObject() && value->contains(name.toString()))
    {
        value = &((*value)[name.toString()]);
    }
    else
    {
        return QScriptValue();
    }

    //create QScriptValue
    if (value->isBoolean())
    {
        return QScriptValue(value->getBoolean());
    }
    else if (value->isInteger())
    {
        return QScriptValue(value->getInteger());
    }
    else if (value->isFloat())
    {
        return QScriptValue(value->getFloat());
    }
    else if (value->isString())
    {
        return QScriptValue(value->getString());
    }
    else if (value->isArray())
    {
        QScriptValue scriptValue = engine()->newVariant(QVariant::fromValue(value));
        QScriptValue wrappedValue = engine()->newObject(this, scriptValue);

        return wrappedValue;
    }
    else if (value->isObject())
    {
        QScriptValue scriptValue = engine()->newVariant(QVariant::fromValue(value));
        QScriptValue wrappedValue = engine()->newObject(this, scriptValue);

        return wrappedValue;
    }

    return QScriptValue();
}

void ValueScriptBinding::setProperty(QScriptValue& object, const QScriptString& name, uint id, const QScriptValue& newValue)
{
    Value* value = qscriptvalue_cast<Value*>(object.data());
    if (!value)
    {
        return;
    }

    //get value from array or object
    if (value->isArray() && name.toArrayIndex() < value->size())
    {
        value = &((*value)[name.toArrayIndex()]);
    }
    else if (value->isObject() && value->contains(name.toString()))
    {
        value = &((*value)[name.toString()]);
    }
    else
    {
        return;
    }

    //set value from QScriptValue
    if (newValue.isBool())
    {
        *value = newValue.toBool();
    }
    else if (newValue.isNumber())
    {
        *value = newValue.toNumber();
    }
    else if (newValue.isString())
    {
        *value = newValue.toString();
    }
    else if (newValue.isArray())
    {
        *value = Value::Array();
        QVariantList list = qscriptvalue_cast<QVariantList>(newValue);
        setPropertyFromArray(list, value);
    }
    else if (newValue.isObject())
    {
        *value = Value::Object();
        QVariantMap map = qscriptvalue_cast<QVariantMap>(newValue);
        setPropertyFromObject(map, value);
    }
}

QScriptClass::QueryFlags ValueScriptBinding::queryProperty(const QScriptValue& object, const QScriptString& name, QScriptClass::QueryFlags flags, uint* id)
{
    if (name.toString() != "toString" && name.toString() != "valueOf")
    {
        return QScriptClass::HandlesReadAccess | QScriptClass::HandlesWriteAccess;
    }

    return 0;
}

QScriptValue ValueScriptBinding::create(QScriptEngine* engine, Value* value)
{
    ValueScriptBinding* valueScriptBinding = new ValueScriptBinding(engine); //TODO will cause memory leak
    QScriptValue scriptValue = engine->newVariant(QVariant::fromValue(value));
    QScriptValue wrappedValue = engine->newObject(valueScriptBinding, scriptValue);

    return wrappedValue;
}

void ValueScriptBinding::setPropertyFromArray(const QVariantList& list, Value* value)
{
    for (int i = 0; i < list.size(); i++)
    {
        const QVariant& v = list.at(i);
        if (v.type() == QVariant::Bool)
        {
            (*value)[i] = v.toBool();
        }
        else if (v.type() == QVariant::Int)
        {
            (*value)[i] = v.toInt();
        }
        else if (v.type() == QVariant::Double)
        {
            (*value)[i] = v.toDouble();
        }
        else if (v.type() == QVariant::String)
        {
            (*value)[i] = v.toString();
        }
        else if (v.type() == QVariant::List)
        {
            QVariantList list = v.toList();
            setPropertyFromArray(list, &(*value)[i]);
        }
        else if (v.type() == QVariant::Map)
        {
            QVariantMap map = v.toMap();
            setPropertyFromObject(map, &(*value)[i]);
        }
    }
}

void ValueScriptBinding::setPropertyFromObject(const QVariantMap& map, Value* value)
{
    QMapIterator<QString, QVariant> it(map);
    while (it.hasNext())
    {
        it.next();
        if (it.value().type() == QVariant::Bool)
        {
            (*value)[it.key()] = it.value().toBool();
        }
        else if (it.value().type() == QVariant::Int)
        {
            (*value)[it.key()] = it.value().toInt();
        }
        else if (it.value().type() == QVariant::Double)
        {
            (*value)[it.key()] = it.value().toDouble();
        }
        else if (it.value().type() == QVariant::String)
        {
            (*value)[it.key()] = it.value().toString();
        }
        else if (it.value().type() == QVariant::List)
        {
            QVariantList list = it.value().toList();
            setPropertyFromArray(list, &(*value)[it.key()]);
        }
        else if (it.value().type() == QVariant::Map)
        {
            QVariantMap map = it.value().toMap();
            setPropertyFromObject(map, &(*value)[it.key()]);
        }
    }
}
