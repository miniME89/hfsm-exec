QT -= core gui

TARGET = yaml-cpp
TEMPLATE = lib
CONFIG += staticlib

SOURCES += src/null.cpp \
           src/emitterstate.h \
           src/setting.h \
           src/scanscalar.cpp \
           src/nodeevents.h \
           src/simplekey.cpp \
           src/collectionstack.h \
           src/scanner.cpp \
           src/scanner.h \
           src/stringsource.h \
           src/ptr_vector.h \
           src/nodeevents.cpp \
           src/singledocparser.h \
           src/node.cpp \
           src/binary.cpp \
           src/scantag.cpp \
           src/scantag.h \
           src/ptr_stack.h \
           src/ostream_wrapper.cpp \
           src/tag.cpp \
           src/scantoken.cpp \
           src/stream.cpp \
           src/nodebuilder.cpp \
           src/emitterutils.h \
           src/scanscalar.h \
           src/singledocparser.cpp \
           src/convert.cpp \
           src/streamcharsource.h \
           src/exp.h \
           src/regex.h \
           src/indentation.h \
           src/directives.h \
           src/parse.cpp \
           src/emitfromevents.cpp \
           src/stream.h \
           src/contrib/graphbuilderadapter.cpp \
           src/contrib/graphbuilder.cpp \
           src/contrib/graphbuilderadapter.h \
           src/emit.cpp \
           src/node_data.cpp \
           src/token.h \
           src/emitter.cpp \
           src/exp.cpp \
           src/emitterstate.cpp \
           src/directives.cpp \
           src/memory.cpp \
           src/tag.h \
           src/regex.cpp \
           src/parser.cpp \
           src/emitterutils.cpp \
           src/nodebuilder.h \
           src/regeximpl.h

HEADERS += inc/eventhandler.h \
           inc/emitfromevents.h \
           inc/traits.h \
           inc/dll.h \
           inc/noncopyable.h \
           inc/node/impl.h \
           inc/node/ptr.h \
           inc/node/type.h \
           inc/node/node.h \
           inc/node/detail \
           inc/node/detail/impl.h \
           inc/node/detail/node_data.h \
           inc/node/detail/node.h \
           inc/node/detail/memory.h \
           inc/node/detail/node_ref.h \
           inc/node/detail/iterator_fwd.h \
           inc/node/detail/bool_type.h \
           inc/node/detail/node_iterator.h \
           inc/node/detail/iterator.h \
           inc/node/iterator.h \
           inc/node/parse.h \
           inc/node/convert.h \
           inc/node/emit.h \
           inc/parser.h \
           inc/binary.h \
           inc/exceptions.h \
           inc/ostream_wrapper.h \
           inc/contrib/anchordict.h \
           inc/contrib/graphbuilder.h \
           inc/stlemitter.h \
           inc/anchor.h \
           inc/emitter.h \
           inc/emitterdef.h \
           inc/emittermanip.h \
           inc/yaml.h \
           inc/null.h \
           inc/mark.h

INCLUDEPATH += inc

#setup build directory
DESTDIR = ../../build/lib/
OBJECTS_DIR = build/obj
