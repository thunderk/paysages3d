TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

TARGET = paysages-experiment-bruneton

include(../../common.pri)

QMAKE_CXXFLAGS -= -std=c++11

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += gl glu glew libtiff-4
    LIBS += -lglut
}

SOURCES += \
    Main.cpp

HEADERS += \
    Main.h \
    mat4.h \
    vec3.h

