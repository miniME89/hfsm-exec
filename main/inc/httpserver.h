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

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <microhttpd.h>
#include <map>
#include <sstream>
#include <mutex>
#include <condition_variable>

namespace hfsmexec
{
    class PushNotification
    {
        public:
            PushNotification(int maxSize = 100);
            ~PushNotification();

            void write(const std::string& data);
            bool read(int& pos, std::string& data, int timeout = 5);

            void unlock();

        private:
            std::map<int, std::string> buffer;
            int pos;
            int maxSize;
            std::mutex lock;
            std::condition_variable condition;
    };

    class HttpRequest
    {
        friend class HttpServer;

        public:
            HttpRequest();
            ~HttpRequest();

            const std::string& getVersion() const;
            const std::string& getMethod() const;
            const std::string& getUrl() const;
            const std::string& getBody() const;
            const std::map<std::string, std::string>& getHeaders() const;
            std::string getHeader(const std::string& key) const;
            void setHeader(const std::string& key, const std::string& value);
            bool hasHeader(const std::string& key);
            const std::map<std::string, std::string>& getArguments() const;
            std::string getArgument(const std::string& key) const;
            void setArgument(const std::string& key, const std::string& value);
            bool hasArgument(const std::string& key);

        private:
            std::string version;
            std::string method;
            std::string url;
            std::string body;
            std::map<std::string, std::string> headers;
            std::map<std::string, std::string> arguments;
    };

    class HttpResponse
    {
        friend class HttpServer;

        public:
            enum StatusCode {
                STATUS_CONTINUE = 100,
                STATUS_SWITCH_PROTOCOLS = 101,
                STATUS_OK = 200,
                STATUS_CREATED = 201,
                STATUS_ACCEPTED = 202,
                STATUS_NON_AUTHORITATIVE_INFORMATION = 203,
                STATUS_NO_CONTENT = 204,
                STATUS_RESET_CONTENT = 205,
                STATUS_PARTIAL_CONTENT = 206,
                STATUS_MULTIPLE_CHOICES = 300,
                STATUS_MOVED_PERMANENTLY = 301,
                STATUS_FOUND = 302,
                STATUS_SEE_OTHER = 303,
                STATUS_NOT_MODIFIED = 304,
                STATUS_USE_PROXY = 305,
                STATUS_TEMPORARY_REDIRECT = 307,
                STATUS_BAD_REQUEST = 400,
                STATUS_UNAUTHORIZED = 401,
                STATUS_PAYMENT_REQUIRED = 402,
                STATUS_FORBIDDEN = 403,
                STATUS_NOT_FOUND = 404,
                STATUS_METHOD_NOT_ALLOWED = 405,
                STATUS_NOT_ACCEPTABLE = 406,
                STATUS_PROXY_AUTHENTICATION_REQUIRED = 407,
                STATUS_REQUEST_TIMEOUT = 408,
                STATUS_CONFLICT = 409,
                STATUS_GONE = 410,
                STATUS_LENGTH_REQUIRED = 411,
                STATUS_PRECONDITION_FAILED = 412,
                STATUS_REQUEST_ENTITY_TOO_LARGE = 413,
                STATUS_REQUEST_URI_TOO_LONG = 414,
                STATUS_REQUEST_UNSUPPORTED_MEDIA_TYPE = 415,
                STATUS_REQUESTED_RANGE_NOT_SATISFIABLE = 416,
                STATUS_EXPECTATION_FAILED = 417,
                STATUS_INTERNAL_SERVER_ERROR = 500,
                STATUS_NOT_IMPLEMENTED = 501,
                STATUS_BAD_GATEWAY = 502,
                STATUS_SERVICE_UNAVAILABLE = 503,
                STATUS_GATEWAY_TIMEOUT = 504,
                STATUS_HTTP_VERSION_NOT_SUPPORTED = 505
            };

            HttpResponse();
            ~HttpResponse();

            const std::map<std::string, std::string>& getHeaders() const;
            std::string getHeader(const std::string& key) const;
            void setHeader(const std::string& key, const std::string& value);
            bool hasHeader(const std::string& key);

            void setStatusCode(int statusCode);
            void write(const std::string& data);

        private:
            int statusCode;
            std::stringstream data;
            std::map<std::string, std::string> headers;
    };

    class Context
    {
        friend class HttpServer;

        public:
            Context();
            ~Context();

            HttpRequest* getRequest();
            HttpResponse* getResponse();

        private:
            HttpRequest* request;
            HttpResponse* response;
    };

    class HttpServer
    {
        public:
            typedef void HandlerCallback(HttpRequest*, HttpResponse*);

            HttpServer();
            ~HttpServer();

            bool start(int port = 8080);
            bool stop();

            void setHandler(const std::function<HandlerCallback>& handler);

        private:
            struct MHD_Daemon* daemon;
            std::function<HandlerCallback> handler;

            static int requestHandler(void* cls, struct MHD_Connection* connection, const char* url, const char* method, const char* version, const char* uploadData, size_t* uploadDataSize, void** conCls);
            static void requestCompleted(void* cls, struct MHD_Connection* connection, void** conCls, enum MHD_RequestTerminationCode toe);
            static int readHeader(void* cls, enum MHD_ValueKind kind, const char* key, const char* value);
            static int readArguments(void* cls, enum MHD_ValueKind kind, const char* key, const char* value);
    };
}

#endif
