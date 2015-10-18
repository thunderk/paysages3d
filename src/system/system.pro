#-------------------------------------------------
#
# Project created by QtCreator 2013-10-20T11:38:08
#
#-------------------------------------------------

#QT       -= gui

TEMPLATE = lib
TARGET = paysages_system

DEFINES += SYSTEM_LIBRARY

include(../common.pri)

SOURCES += $$files(*.cpp)
HEADERS += $$files(*.h)

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
