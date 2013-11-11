#-------------------------------------------------
#
# Project created by QtCreator 2013-10-29T17:15:02
#
#-------------------------------------------------

QT       -= gui

TARGET = paysages_basics
TEMPLATE = lib

DEFINES += BASICS_LIBRARY

include(../common.pri)

SOURCES += \
    NoiseGenerator.cpp \
    NoiseFunctionNaive.cpp \
    NoiseFunctionPerlin.cpp \
    NoiseFunctionSimplex.cpp \
    Interpolation.cpp \
    SpaceCoordinates.cpp \
    Vector3.cpp \
    Vector3.inline.cpp

HEADERS +=\
        basics_global.h \
    NoiseGenerator.h \
    NoiseFunctionNaive.h \
    NoiseFunctionPerlin.h \
    NoiseFunctionSimplex.h \
    Interpolation.h \
    SpaceCoordinates.h \
    Vector3.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../system/release/ -lpaysages_system
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../system/debug/ -lpaysages_system
else:unix: LIBS += -L$$OUT_PWD/../system/ -lpaysages_system
INCLUDEPATH += $$PWD/../system
DEPENDPATH += $$PWD/../system
