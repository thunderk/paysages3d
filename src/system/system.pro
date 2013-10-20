#-------------------------------------------------
#
# Project created by QtCreator 2013-10-20T11:38:08
#
#-------------------------------------------------

#QT       -= gui

TEMPLATE = lib
TARGET = paysages_system

DEFINES += SYSTEM_LIBRARY
CONFIG(release, debug|release): DEFINES += NDEBUG

SOURCES += \
    PictureFile.cpp \
    Thread.cpp \
    Mutex.cpp

HEADERS += \
    system_global.h \
    PictureFile.h \
    Thread.h \
    Mutex.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
