QT       += core xml xmlpatterns
QT       -= gui

TARGET = hfsm-exec

CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES +=  src/main.cpp \
            src/application.cpp \
            src/api.cpp \
            src/statemachine.cpp \
            src/statemachine_impl.cpp \
            src/decoder.cpp \
            src/decoder_impl.cpp \
            src/parameter_server.cpp \
            src/plugins.cpp

HEADERS += inc/application.h \
           inc/api.h \
           inc/statemachine.h \
           inc/statemachine_impl.h \
           inc/decoder.h \
           inc/decoder_impl.h  \
           inc/parameter_server.h \
           inc/plugins.h

INCLUDEPATH += inc

LIBS += -L/usr/local/lib/ -lbooster -lcppcms

INCLUDEPATH += /usr/local/include/

release: DESTDIR = build/release
debug:   DESTDIR = build/debug

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui
