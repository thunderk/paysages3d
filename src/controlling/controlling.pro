include(../common.pri)

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

TARGET = paysages-cli

OBJECTS_DIR = $$DESTDIR/controlling/

DEPENDPATH += ..
linux-clang:LIBS += -L$$DESTDIR -lpaysages_rendering

SOURCES += main.c

