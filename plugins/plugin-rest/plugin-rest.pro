QT -= gui

TARGET = plugin-rest
TEMPLATE = lib

DEFINES += REST_LIBRARY

SOURCES += \
    plugin_rest.cpp

HEADERS += \
    plugin_rest.h

INCLUDEPATH += ../../inc/

unix {
    target.path = /usr/lib
    INSTALLS += target
}
