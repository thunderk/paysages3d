TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

TARGET = paysages-cli
INCLUDEPATH += ..

CONFIG(debug) {
    DESTDIR = ../../build/debug/
    MAKEFILE = Makefile.debug
} else {
    DESTDIR = ../../build/release/
    MAKEFILE = Makefile.release
}

OBJECTS_DIR = $$DESTDIR/controlling/

release:DEFINES += NDEBUG

DEPENDPATH += ..
linux-clang:LIBS += -L$$DESTDIR -lpaysages_rendering

SOURCES += main.c

