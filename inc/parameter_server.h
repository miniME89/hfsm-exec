#ifndef PARAMETER_SERVER_H
#define PARAMETER_SERVER_H

#include <QString>

#include <cppcms/json.h>

#include <string>

namespace hfsmexec
{
    typedef bool Bool;
    typedef double Number;
    typedef std::string String;
    typedef cppcms::json::array Array;
    typedef cppcms::json::object Object;
    typedef cppcms::json::value Value;

    class ParameterServer
    {
        public:
            static const std::string typeName[7];

            ParameterServer();

            template<typename T>
            bool getParameter(const QString& path, T& value);
            bool getParameter(const QString& path, Bool& value);
            bool getParameter(const QString& path, Number& value);
            bool getParameter(const QString& path, String& value);
            bool getParameter(const QString& path, Array& value);
            bool getParameter(const QString& path, Object& value);

            template<typename T>
            void setParameter(const QString& path, T const& value);
            void setParameter(const QString& path, Bool& value);
            void setParameter(const QString& path, Number& value);
            void setParameter(const QString& path, String& value);
            void setParameter(const QString& path, Array& value);
            void setParameter(const QString& path, Object& value);

            void deleteParameter(const QString& path);

            QString toXml(const QString& path);
            QString toJson(const QString& path);
            QString toYaml(const QString& path);

            bool fromXml(const QString& path, const QString& xml);
            bool fromJson(const QString& path, const QString& json);
            bool fromYaml(const QString& path, const QString& json);

        private:
            Value parameters;

            bool getValue(const QString& path, Value*& value);
            bool getValue(const QString& path, Value const*& value);
    };

    class ParameterServerTest
    {
        public:
            ParameterServerTest();
    };
}

#endif
