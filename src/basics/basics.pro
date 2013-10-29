#-------------------------------------------------
#
# Project created by QtCreator 2013-10-29T17:15:02
#
#-------------------------------------------------

QT       -= gui

TARGET = basics
TEMPLATE = lib

DEFINES += BASICS_LIBRARY

SOURCES +=

HEADERS +=\
        basics_global.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
