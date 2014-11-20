QT -= core gui

TARGET = pugixml
TEMPLATE = lib
CONFIG += staticlib

SOURCES += src/pugixml.cpp
HEADERS += inc/pugixml.h

INCLUDEPATH += inc

#setup build directory
DESTDIR = ../../build/lib/
OBJECTS_DIR = build/obj
