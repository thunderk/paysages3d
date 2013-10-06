QT += opengl

TEMPLATE = lib
CONFIG += console

TARGET = paysages_exploring

CONFIG(release, debug|release): DEFINES += NDEBUG

INCLUDEPATH += $$PWD/..

SOURCES += main.c

HEADERS += main.h
