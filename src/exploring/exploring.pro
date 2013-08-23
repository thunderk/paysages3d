include(../common.pri)

TEMPLATE = lib
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

TARGET = paysages_exploring

OBJECTS_DIR = $$DESTDIR/exploring/

SOURCES += main.c \
    terrain.c \
    sky.c \
    chunk.c

HEADERS += main.h \
    terrain.h \
    sky.h \
    chunk.h

