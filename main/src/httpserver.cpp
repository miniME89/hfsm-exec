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

#include <httpserver.h>

using namespace hfsmexec;

/*
 * PushNotification
 */
PushNotification::PushNotification(int maxQueueSize, int maxReadSize) :
    pos(0),
    maxQueueSize(maxQueueSize),
    maxReadSize(maxReadSize) {

}

PushNotification::~PushNotification() {

}

void PushNotification::write(const std::string& data) {
    {
        std::lock_guard<std::mutex> scopedLock(lock);

        if (this->pos == -1) {
            return;
        }

        pos++;
        buffer[pos] = data;
        if (pos > maxQueueSize) {
            buffer.erase(buffer.find(pos - maxQueueSize));
        }
    }

    condition.notify_all();
}

bool PushNotification::read(int& pos, std::string& data, int timeout) {
    // set pos to buffer end, if pos <= 0
    if (pos <= 0) {
        pos = this->pos + 1;
    }

    // wait till message at buffer pos is available
    std::unique_lock<std::mutex> conditionLock(lock);
    if (!condition.wait_for(conditionLock, std::chrono::seconds(timeout), [&] {return pos <= this->pos || this->pos == -1;})) {
        return false;
    }

    // verify that buffer is still valid
    if (this->pos == -1) {
        conditionLock.unlock();
        condition.notify_all();

        return false;
    }

    // make sure that a valid buffer pos is selected
    int bufferStart = this->pos - maxQueueSize + 1;
    if (pos < bufferStart) {
        pos = bufferStart;
    }

    // read all available buffer from pos (max
    data.append("[");
    int bufferEnd = this->pos;
    for (int i = 0; pos <= bufferEnd && i < maxReadSize; i++, pos++) {
        if (i > 0) {
            data.append(", ");
        }

        data.append(buffer[pos]);
    }
    data.append("]");

    conditionLock.unlock();
    condition.notify_all();

    return true;
}

void PushNotification::unlock() {
    this->pos = -1;
    condition.notify_all();
}

/*
 * HttpRequest
 */
HttpRequest::HttpRequest() {

}

HttpRequest::~HttpRequest() {

}

const std::string& HttpRequest::getVersion() const {
    return version;
}

const std::string& HttpRequest::getMethod() const {
    return method;
}

const std::string& HttpRequest::getUrl() const {
    return url;
}

const std::string& HttpRequest::getBody() const {
    return body;
}

const std::map<std::string, std::string>& HttpRequest::getHeaders() const {
    return headers;
}

std::string HttpRequest::getHeader(const std::string& key) const {
    std::map<std::string, std::string>::const_iterator i = headers.find(key);

    if (i == headers.end()) {
        return "";
    }

    return i->second;
}

void HttpRequest::setHeader(const std::string& key, const std::string& value) {
    headers.insert(std::pair<std::string, std::string>(key, value));
}

bool HttpRequest::hasHeader(const std::string& key) {
    return headers.find(key) != headers.end();
}

const std::map<std::string, std::string>& HttpRequest::getArguments() const {
    return arguments;
}

std::string HttpRequest::getArgument(const std::string& key) const {
    std::map<std::string, std::string>::const_iterator i = arguments.find(key);

    if (i == arguments.end()) {
        return "";
    }

    return i->second;
}

void HttpRequest::setArgument(const std::string& key, const std::string& value) {
    arguments.insert(std::pair<std::string, std::string>(key, value));
}

bool HttpRequest::hasArgument(const std::string& key) {
    return arguments.find(key) != arguments.end();
}

/*
 * HttpResponse
 */
HttpResponse::HttpResponse() :
    statusCode(STATUS_NOT_FOUND) {

}

HttpResponse::~HttpResponse() {

}

void HttpResponse::setStatusCode(int statusCode) {
    this->statusCode = statusCode;
}

void HttpResponse::write(const std::string& data) {
    this->data <<data;
}

const std::map<std::string, std::string>& HttpResponse::getHeaders() const {
    return headers;
}

std::string HttpResponse::getHeader(const std::string& key) const {
    std::map<std::string, std::string>::const_iterator i = headers.find(key);

    if (i == headers.end()) {
        return "";
    }

    return i->second;
}

void HttpResponse::setHeader(const std::string& key, const std::string& value) {
    headers.insert(std::pair<std::string, std::string>(key, value));
}

bool HttpResponse::hasHeader(const std::string& key) {
    return headers.find(key) != headers.end();
}

/*
 * Context
 */
Context::Context() {

}

Context::~Context() {
    delete request;
    delete response;
}

HttpRequest* Context::getRequest() {
    return request;
}

HttpResponse* Context::getResponse() {
    return response;
}

/*
 * HttpServer
 */
HttpServer::HttpServer() {

}

HttpServer::~HttpServer() {

}

bool HttpServer::start(int port) {
    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY | MHD_USE_THREAD_PER_CONNECTION, port, NULL, NULL, requestHandler, this, MHD_OPTION_NOTIFY_COMPLETED, requestCompleted, NULL, MHD_OPTION_END);

    return daemon != NULL;
}

bool HttpServer::stop() {
    MHD_stop_daemon(daemon);

    return true;
}

void HttpServer::setHandler(const std::function<HttpServer::HandlerCallback>& handler) {
    this->handler = handler;
}

int HttpServer::requestHandler(void* cls, struct MHD_Connection* connection, const char* url, const char* method, const char* version, const char* uploadData, size_t* uploadDataSize, void** conCls) {
    // create context
    if (*conCls == NULL) {
        HttpRequest* request = new HttpRequest();
        request->version = version;
        request->method = method;
        request->url = url;

        HttpResponse* response = new HttpResponse();

        Context* context = new Context();
        context->request = request;
        context->response = response;

        *conCls = (void*)context;

        return MHD_YES;
    }

    Context* context = (Context*)*conCls;
    HttpRequest* request = context->getRequest();
    HttpResponse* response = context->getResponse();

    // process upload data
    if (*uploadDataSize != 0) {
        context->request->body = uploadData;
        *uploadDataSize = 0;

        return MHD_YES;
    }

    // read headers
    MHD_get_connection_values(connection, MHD_HEADER_KIND, readHeader, context);

    // read URI arguments
    MHD_get_connection_values(connection, MHD_GET_ARGUMENT_KIND, readArguments, context);

    // handle request
    HttpServer* server = (HttpServer*)cls;
    if (server->handler) {
        server->handler(request, response);
    }

    // create response
    std::string dataStr = response->data.str();
    const char* data = dataStr.c_str();
    struct MHD_Response* responseMHD = MHD_create_response_from_buffer(dataStr.size(), (void*)data, MHD_RESPMEM_MUST_COPY);

    // set response headers
    std::map<std::string, std::string> responseHeaders = response->getHeaders();
    for (std::map<std::string, std::string>::iterator i = responseHeaders.begin(); i != responseHeaders.end(); i++) {
        MHD_add_response_header(responseMHD, i->first.c_str(), i->second.c_str());
    }

    // send response
    int ret = MHD_queue_response(connection, response->statusCode, responseMHD);

    MHD_destroy_response(responseMHD);

    return ret;
}

void HttpServer::requestCompleted(void* cls, MHD_Connection* connection, void** conCls, MHD_RequestTerminationCode toe) {
    Context* context = (Context*)*conCls;

    if (context == NULL) {
        return;
    }

    delete context;

    *conCls = NULL;
}

int HttpServer::readHeader(void* cls, MHD_ValueKind kind, const char* key, const char* value) {
    Context* context = (Context*)cls;
    context->request->headers.insert(std::pair<std::string, std::string>(key, value));

    return MHD_YES;
}

int HttpServer::readArguments(void* cls, MHD_ValueKind kind, const char* key, const char* value) {
    Context* context = (Context*)cls;
    context->request->arguments.insert(std::pair<std::string, std::string>(key, value));

    return MHD_YES;
}
