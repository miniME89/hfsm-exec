QT -= gui

TARGET = plugin-dot
TEMPLATE = lib

DEFINES += HTTP_LIBRARY

SOURCES += src/plugin_dot.cpp

HEADERS += inc/plugin_dot.h

INCLUDEPATH += inc
INCLUDEPATH += ../../../main/inc/

#setup build directory
DESTDIR = ../../../build/plugins/
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
