QT += core xml xmlpatterns
QT -= gui

TARGET = hfsm-exec

CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += src/main.cpp \
           src/application.cpp \
           src/api.cpp \
           src/statemachine.cpp \
           src/statemachine_impl.cpp \
           src/decoder.cpp \
           src/decoder_impl.cpp \
           src/value_container.cpp \
           src/plugins.cpp

HEADERS += inc/application.h \
           inc/api.h \
           inc/statemachine.h \
           inc/statemachine_impl.h \
           inc/decoder.h \
           inc/decoder_impl.h \
           inc/value_container.h \
           inc/plugins.h

INCLUDEPATH += inc

#external applications
INCLUDEPATH += ../ext/pugixml/inc/ ../ext/jsoncpp/inc/ ../ext/yaml-cpp/inc/
LIBS += -L../build/lib/ -lpugixml -ljsoncpp -lyaml-cpp

#cppcms
INCLUDEPATH += /usr/local/include/
LIBS += -L/usr/local/lib/ -lbooster -lcppcms

#linker flags
QMAKE_LFLAGS += -export-dynamic

#setup build directory
DESTDIR = ../build/
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
