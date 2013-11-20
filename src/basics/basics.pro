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
    Vector3.cpp \
    Vector3.inline.cpp \
    SpaceSegment.cpp \
    Color.cpp \
    Color.inline.cpp \
    ColorHSL.cpp \
    BoundingBox.cpp \
    Matrix4.cpp \
    Curve.cpp \
    ColorProfile.cpp \
    Geometry.cpp

HEADERS +=\
        basics_global.h \
    NoiseGenerator.h \
    NoiseFunctionNaive.h \
    NoiseFunctionPerlin.h \
    NoiseFunctionSimplex.h \
    Interpolation.h \
    Vector3.h \
    SpaceSegment.h \
    Color.h \
    ColorHSL.h \
    BoundingBox.h \
    Matrix4.h \
    Curve.h \
    ColorProfile.h \
    Geometry.h

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
