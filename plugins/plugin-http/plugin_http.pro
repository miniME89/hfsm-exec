QT -= gui

TARGET = plugin-http
TEMPLATE = lib

DEFINES += HTTP_LIBRARY

SOURCES += plugin_http.cpp

HEADERS += plugin_http.h

INCLUDEPATH += ../../inc/

unix {
    target.path = /usr/lib
    INSTALLS += target
}
