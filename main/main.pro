QT += core network
QT -= gui

TARGET = hfsm-exec

QMAKE_CXXFLAGS += -std=c++11

CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += src/main.cpp \
           src/logger.cpp \
           src/application.cpp \
           src/api.cpp \
           src/statemachine.cpp \
           src/statemachine_impl.cpp \
           src/decoder.cpp \
           src/decoder_impl.cpp \
           src/value_container.cpp \
           src/plugins.cpp

HEADERS += inc/logger.h \
           inc/application.h \
           inc/api.h \
           inc/statemachine.h \
           inc/statemachine_impl.h \
           inc/decoder.h \
           inc/decoder_impl.h \
           inc/value_container.h \
           inc/plugins.h

INCLUDEPATH += inc

INCLUDEPATH += ../ext/easylogging++/inc/
LIBS += -lpugixml -ljsoncpp -lyaml-cpp
LIBS += -lcppcms

#linker flags
QMAKE_LFLAGS += -export-dynamic

#setup build directory
DESTDIR = ../build/
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
