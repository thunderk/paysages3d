TEMPLATE = lib
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

TARGET = paysages_exploring
INCLUDEPATH += ..

CONFIG(debug) {
    DESTDIR = ../../build/debug/
    MAKEFILE = Makefile.debug
} else {
    DESTDIR = ../../build/release/
    MAKEFILE = Makefile.release
}

OBJECTS_DIR = $$DESTDIR/exploring/

release:DEFINES += NDEBUG

SOURCES += main.c \
    terrain.c \
    sky.c \
    chunk.c

HEADERS += main.h \
    terrain.h \
    sky.h \
    chunk.h

