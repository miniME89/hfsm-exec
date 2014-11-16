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

#include <parameter_container.h>

#include <QDebug>
#include <QList>
#include <QRegExp>

#include <sstream>

using namespace hfsmexec;

const QString ParameterContainer::typeName[7] = {"undefined", "null", "boolean", "number", "string", "object", "array"};

ParameterContainer::ParameterContainer() :
    value(new Value())
{

}

ParameterContainer::ParameterContainer(const ParameterContainer& other) :
    value(new Value(*other.getValue()))
{

}

ParameterContainer::~ParameterContainer()
{
    delete value;
}

const Value* ParameterContainer::getValue() const
{
    return value;
}

template<typename T>
T ParameterContainer::get(const QString& path, T defaultValue) const
{
    T value;
    get<T>(path, value, defaultValue);

    return value;
}

Bool ParameterContainer::get(const QString& path, Bool defaultValue) const
{
    return get<Bool>(path, defaultValue);
}

Number ParameterContainer::get(const QString& path, Number defaultValue) const
{
    return get<Number>(path, defaultValue);
}

String ParameterContainer::get(const QString& path, String defaultValue) const
{
    return get<String>(path, defaultValue);
}

Array ParameterContainer::get(const QString& path, Array defaultValue) const
{
    return get<Array>(path, defaultValue);
}

Object ParameterContainer::get(const QString& path, Object defaultValue) const
{
    return get<Object>(path, defaultValue);
}

Value ParameterContainer::get(const QString& path, Value defaultValue) const
{
    Value value;
    get(path, value, defaultValue);

    return value;
}

template<typename T>
bool ParameterContainer::get(const QString& path, T& value, T defaultValue) const
{
    try
    {
        Value const* parameter;
        if (getValue(path, parameter))
        {
            value = (*parameter).get_value<T>();

            return true;
        }
        else
        {
            value = defaultValue;
        }
    }
    catch(...)
    {
    }

    return false;
}

bool ParameterContainer::get(const QString& path, Bool& value, Bool defaultValue) const
{
    return get<Bool>(path, value, defaultValue);
}

bool ParameterContainer::get(const QString& path, Number& value, Number defaultValue) const
{
    return get<Number>(path, value, defaultValue);
}

bool ParameterContainer::get(const QString& path, String& value, String defaultValue) const
{
    return get<String>(path, value, defaultValue);
}

bool ParameterContainer::get(const QString& path, Array& value, Array defaultValue) const
{
    return get<Array>(path, value, defaultValue);
}

bool ParameterContainer::get(const QString& path, Object& value, Object defaultValue) const
{
    return get<Object>(path, value, defaultValue);
}

bool ParameterContainer::get(const QString& path, Value& value, Value defaultValue) const
{
    try
    {
        Value const* parameter;
        if (getValue(path, parameter))
        {
            value = *parameter;

            return true;
        }
        else
        {
            value = defaultValue;
        }
    }
    catch(...)
    {
    }

    return false;
}

template<typename T>
void ParameterContainer::set(const QString& path, const T& value)
{
    Value* parameter;
    if (getValue(path, parameter))
    {
        if (parameter != this->value)
        {
            *parameter = value;
        }
    }
}

void ParameterContainer::set(const QString& path, const Bool& value)
{
    set<Bool>(path, value);
}

void ParameterContainer::set(const QString& path, const Number& value)
{
    set<Number>(path, value);
}

void ParameterContainer::set(const QString& path, const String& value)
{
    set<String>(path, value);
}

void ParameterContainer::set(const QString& path, const Array& value)
{
    set<Array>(path, value);
}

void ParameterContainer::set(const QString& path, const Object& value)
{
    set<Object>(path, value);
}

void ParameterContainer::set(const QString& path, const Value& value)
{
    set<Value>(path, value);
}

void ParameterContainer::remove(const QString& path)
{
    Value* parameter;
    if (getValue(path, parameter))
    {
        parameter->undefined(); //TODO
    }
}

bool ParameterContainer::toXml(const QString& path, QString& xml, XmlFormat format) const
{
    Value const* parameter;
    if (getValue(path, parameter))
    {
        if (parameter->type() == cppcms::json::is_boolean)
        {
            Bool b = parameter->boolean();
            xml.append((b) ? "true" : "false");
        }
        else if (parameter->type() == cppcms::json::is_number)
        {
            Number num = parameter->number();
            xml.append(QString::number(num));
        }
        else if (parameter->type() == cppcms::json::is_string)
        {
            String str = parameter->str();
            xml.append(QString(str.c_str()));
        }
        else if (parameter->type() == cppcms::json::is_object)
        {
            Object const& obj = parameter->object();
            for (Object::const_iterator it = obj.begin(); it != obj.end(); it++)
            {
                QString key = QString(it->first.str().c_str());
                QString type = typeName[it->second.type()];
                QString tagOpen;
                QString tagClose;

                if (format == NAME_TAG)
                {
                    tagOpen = QString("<%1 type=\"%2\">").arg(key).arg(type);
                    tagClose = QString("</%1>").arg(key);
                }
                else if (format == PARAMETER_TAG)
                {
                    tagOpen = QString("<parameter name=\"%1\" type=\"%2\">").arg(key).arg(type);
                    tagClose = QString("</parameter>");
                }

                xml.append(tagOpen);
                toXml(QString(path).append("/").append(key), xml, format);
                xml.append(tagClose);
            }
        }
        else if (parameter->type() == cppcms::json::is_array)
        {
            Array const& arr = parameter->array();
            for (unsigned int i = 0; i != arr.size(); i++)
            {
                QString type = typeName[(int)arr[i].type()];
                QString tagOpen = QString("<item type=\"%1\">").arg(type);
                QString tagClose = QString("</item>");

                xml.append(tagOpen);
                toXml(QString(path).append("[").append(QString::number(i)).append("]"), xml, format);
                xml.append(tagClose);
            }
        }
    }

    return true;
}

bool ParameterContainer::toJson(const QString& path, QString& json) const
{
    Value const* value;
    if (getValue(path, value))
    {
        try
        {
            std::string jsonStr = value->save(cppcms::json::readable);
            json.append(jsonStr.c_str());
        }
        catch(...)
        {
            return false;
        }
    }

    return true;
}

bool ParameterContainer::toYaml(const QString& path, QString& yaml) const
{
    //TODO
}

bool ParameterContainer::fromXml(const QString& path, const QString& xml, XmlFormat format)
{
    Value* value;
    if (getValue(path, value))
    {
        if (value->type() == cppcms::json::is_undefined)
        {
            value->set_value<Object>(Object());
        }

        QString errorMessage;
        int errorLine;
        QDomDocument doc;
        if (!doc.setContent(xml, &errorMessage, &errorLine))
        {
            qWarning() <<"couldn't set parameters from XML string:" <<errorMessage <<"[line: " <<errorLine <<"]";

            return false;
        }

        QDomElement element = doc.documentElement();
        Value newValue;
        if (fromXml(newValue, element, format))
        {
            qWarning() <<"couldn't set parameters from XML string: invalid parameter structure";

            return false;
        }

        *value = newValue;

        return true;
    }
    else
    {
        qWarning() <<"couldn't set parameters from JSON string: unable to get parameter from parameter path" <<path;

        return false;
    }
}

bool ParameterContainer::fromJson(const QString& path, const QString& json)
{
    Value* value;
    if (getValue(path, value))
    {
        if (value->type() != cppcms::json::is_object)
        {
            value->set_value<Object>(Object());
        }

        int errorLine;
        std::istringstream stream(json.toStdString());
        bool success = value->load(stream, true, &errorLine);

        if (!success)
        {
            qWarning() <<"couldn't set parameters from JSON string: error parsing JSON in line" <<errorLine;
        }

        return success;
    }
    else
    {
        qWarning() <<"couldn't set parameters from JSON string: unable to get parameter from parameter path" <<path;

        return false;
    }
}

bool ParameterContainer::fromYaml(const QString& path, const QString& yaml)
{
    //TODO
}

bool ParameterContainer::getValue(const QString& path, Value*& value)
{
    QString replacePath = path.trimmed().replace("[", "/[");
    QStringList splitPath = replacePath.split("/", QString::SkipEmptyParts);

    value = this->value;
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
            std::string key = splitPath[i].toStdString();
            value = &(*value)[key];
        }
    }

    return true;
}

bool ParameterContainer::getValue(const QString& path, Value const*& value) const
{
    QString replacePath = path.trimmed().replace("[", "/[");
    QStringList splitPath = replacePath.split("/", QString::SkipEmptyParts);

    try
    {
        value = this->value;
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
                std::string key = splitPath[i].toStdString();
                value = &(*value)[key];
            }
        }
    }
    catch(...)
    {
        return false;
    }

    return true;
}

bool ParameterContainer::fromXml(Value& value, QDomElement& element, ParameterContainer::XmlFormat format)
{
    QString name;
    QString type = element.attribute("type");
    if (format == NAME_TAG)
    {
        name = element.tagName();
    }
    else if (format == PARAMETER_TAG)
    {
        name = element.attribute("name");
    }

    Value& newValue = value[name.toStdString()];

    if (type == typeName[2])
    {
        newValue = (element.text() == "true") ? true : false;
    }
    else if (type == typeName[3])
    {
        newValue = element.text().toDouble();
    }
    else if (type == typeName[4])
    {
        newValue = element.text().toStdString();
    }
    else if (type == typeName[5])
    {
        QDomElement child = element.firstChild().toElement();
        while (!child.isNull())
        {
            fromXml(newValue, child, format);

            child = child.nextSibling().toElement();
        }
    }
    else if (type == typeName[6])
    {
        QDomElement child = element.firstChild().toElement();
        while (!child.isNull())
        {
            fromXml(newValue, child, format);

            child = child.nextSibling().toElement();
        }
    }
}

ParameterServerTest::ParameterServerTest()
{
    ParameterContainer container;
    container.set("/some/foo", Value("test2"));
    container.set("/some/path/x", 5);
    container.set("/some/path/y", 2);
    container.set("/some/path/test/str", "some string");

    //set from xml
    QString in("<parameter name=\"some\" type=\"object\"><parameter name=\"foo\" type=\"string\">test2</parameter><parameter name=\"path\" type=\"object\">"
               "<parameter name=\"test\" type=\"object\"><parameter name=\"str\" type=\"string\">some string</parameter></parameter>"
               "<parameter name=\"x\" type=\"number\">5</parameter><parameter name=\"y\" type=\"number\">2</parameter></parameter></parameter>");
    container.fromXml("/", in);

    //to xml
    QString xml;
    container.toXml("/", xml, ParameterContainer::PARAMETER_TAG);
    qDebug() <<xml;

    //to json
    QString json;
    container.toJson("/", json);
    qDebug() <<json;

    //copy test
    qDebug() <<"test parameter container copy";
    ParameterContainer c1;
    c1.set("/some_value", 123456);
    ParameterContainer c2(c1);

    qDebug() <<c1.get("/some_value", 0) <<c2.get("/some_value", 0);
    c1.set("/some_value", 654321);
    qDebug() <<c1.get("/some_value", 0) <<c2.get("/some_value", 0);
}
