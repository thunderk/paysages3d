TEMPLATE = app
CONFIG -= app_bundle

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


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../system/release/ -lpaysages_system
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../system/debug/ -lpaysages_system
else:unix: LIBS += -L$$OUT_PWD/../../system/ -lpaysages_system
INCLUDEPATH += $$PWD/../../system
DEPENDPATH += $$PWD/../../system

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../basics/release/ -lpaysages_basics
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../basics/debug/ -lpaysages_basics
else:unix: LIBS += -L$$OUT_PWD/../../basics/ -lpaysages_basics
INCLUDEPATH += $$PWD/../../basics
DEPENDPATH += $$PWD/../../basics
