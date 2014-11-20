QT -= gui

TARGET = plugin-http
TEMPLATE = lib

DEFINES += HTTP_LIBRARY

SOURCES += src/plugin_http.cpp

HEADERS += inc/plugin_http.h

INCLUDEPATH += inc
INCLUDEPATH += ../../main/inc/

#setup build directory
DESTDIR = ../../build/plugins/
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
