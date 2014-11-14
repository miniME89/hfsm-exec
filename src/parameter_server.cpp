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

#include <parameter_server.h>

#include <QDebug>
#include <QList>
#include <QRegExp>
#include <QtXml>

#include <sstream>

using namespace hfsmexec;

const std::string ParameterServer::typeName[7] = {"undefined", "null", "boolean", "number", "string", "object", "array"};

ParameterServer::ParameterServer()
{
    parameters.set_value<Object>(Object());
}

template<typename T>
bool ParameterServer::getParameter(const QString &path, T& value)
{
    try
    {
        Value const* parameter;
        if (getValue(path, parameter))
        {
            value = (*parameter).get_value<T>();

            return true;
        }
    }
    catch(...)
    {
    }

    return false;
}

bool ParameterServer::getParameter(const QString& path, Bool& value)
{
    return getParameter<Bool>(path, value);
}

bool ParameterServer::getParameter(const QString& path, Number& value)
{
    return getParameter<Number>(path, value);
}

bool ParameterServer::getParameter(const QString& path, String& value)
{
    return getParameter<String>(path, value);
}

bool ParameterServer::getParameter(const QString& path, Array& value)
{
    return getParameter<Array>(path, value);
}

bool ParameterServer::getParameter(const QString& path, Object& value)
{
    return getParameter<Object>(path, value);
}

bool ParameterServer::getParameter(const QString& path, Value& value)
{
    try
    {
        Value const* parameter;
        if (getValue(path, parameter))
        {
            value = *parameter;

            return true;
        }
    }
    catch(...)
    {
    }

    return false;
}

template<typename T>
void ParameterServer::setParameter(const QString &path, const T &value)
{
    Value* parameter;
    if (getValue(path, parameter))
    {
        if (parameter != &parameters)
        {
            *parameter = value;
        }
    }
}

void ParameterServer::setParameter(const QString& path, Bool& value)
{
    setParameter<Bool>(path, value);
}

void ParameterServer::setParameter(const QString& path, Number& value)
{
    setParameter<Number>(path, value);
}

void ParameterServer::setParameter(const QString& path, String& value)
{
    setParameter<String>(path, value);
}

void ParameterServer::setParameter(const QString& path, Array& value)
{
    setParameter<Array>(path, value);
}

void ParameterServer::setParameter(const QString& path, Object& value)
{
    setParameter<Object>(path, value);
}

void ParameterServer::setParameter(const QString &path, Value& value)
{
    setParameter<Value>(path, value);
}

void ParameterServer::deleteParameter(const QString& path)
{
    Value* parameter;
    if (getValue(path, parameter))
    {
        parameter->undefined(); //TODO
    }
}

QString ParameterServer::toXml(const QString &path)
{
    QString xml;

    Value const* value;
    if (getValue(path, value))
    {
        if (value->type() == cppcms::json::is_boolean)
        {
            Bool b = value->boolean();
            xml.append((b) ? "true" : "false");
        }
        else if (value->type() == cppcms::json::is_number)
        {
            Number num = value->number();
            xml.append(QString::number(num));
        }
        else if (value->type() == cppcms::json::is_string)
        {
            String str = value->str();
            xml.append(QString(str.c_str()));
        }
        else if (value->type() == cppcms::json::is_object)
        {
            Object const& obj = value->object();
            for (Object::const_iterator it = obj.begin(); it != obj.end(); it++)
            {
                QString key = QString(it->first.str().c_str());
                QString type = QString(typeName[it->second.type()].c_str());
                xml.append(QString("<parameter type=\"%1\" name=\"%2\">").arg(type).arg(key));
                xml.append(toXml(QString(path).append("/").append(key)));
                xml.append(QString("</parameter>"));
            }
        }
        else if (value->type() == cppcms::json::is_array)
        {
            Array const& arr = value->array();
            for (unsigned int i = 0; i != arr.size(); i++)
            {
                QString type = QString(typeName[(int)arr[i].type()].c_str());
                xml.append(QString("<item type=\"%1\">").arg(type));
                xml.append(toXml(QString(path).append("[").append(QString::number(i)).append("]")));
                xml.append(QString("</item>"));
            }
        }
    }

    return xml;
}

QString ParameterServer::toJson(const QString &path)
{
    QString json;

    Value const* value;
    if (getValue(path, value))
    {
        std::string jsonStr = value->save(cppcms::json::readable);
        json.append(jsonStr.c_str());
    }

    return json;
}

QString ParameterServer::toYaml(const QString &path)
{
    //TODO
}

bool ParameterServer::fromXml(const QString& path, const QString& xml)
{
    Value* value;
    if (getValue(path, value))
    {
        if (value->type() == cppcms::json::is_undefined)
        {
            value->set_value<Object>(Object());
        }

        //TODO

        return false;
    }
    else
    {
        qWarning() <<"couldn't set parameters from JSON string: unable to get parameter from parameter path" <<path;

        return false;
    }
}

bool ParameterServer::fromJson(const QString& path, const QString& json)
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

bool ParameterServer::fromYaml(const QString &path, const QString &json)
{
    //TODO
}

bool ParameterServer::getValue(const QString &path, Value*& value)
{
    QString replacePath = path.trimmed().replace("[", "/[");
    QStringList splitPath = replacePath.split("/", QString::SkipEmptyParts);

    value = &parameters;
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

bool ParameterServer::getValue(const QString &path, Value const*& value)
{
    QString replacePath = path.trimmed().replace("[", "/[");
    QStringList splitPath = replacePath.split("/", QString::SkipEmptyParts);

    try
    {
        value = &parameters;
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

ParameterServerTest::ParameterServerTest()
{
    ParameterServer server;
    server.setParameter("/some/foo", Value("test2"));
    server.setParameter("/some/path/x", 5);
    server.setParameter("/some/path/y", 2);
    server.setParameter("/some/path/test/str", "some string");

    qDebug() <<server.toJson("/");

    Value v;
    server.getParameter("/some/foo", v);
    qDebug() <<QString(v.str().c_str());
}
