QT -= core gui

TARGET = jsoncpp
TEMPLATE = lib
CONFIG += staticlib

SOURCES += src/jsoncpp.cpp
HEADERS += inc/jsoncpp.h

INCLUDEPATH += inc

#setup build directory
DESTDIR = ../../build/lib/
OBJECTS_DIR = build/obj
