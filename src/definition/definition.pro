#-------------------------------------------------
#
# Project created by QtCreator 2013-10-29T14:38:43
#
#-------------------------------------------------

QT       -= gui

TARGET = paysages_definition
TEMPLATE = lib

DEFINES += DEFINITION_LIBRARY

include(../common.pri)

SOURCES += \
    BaseDefinition.cpp \
    Layers.cpp \
    LegacyLayer.cpp \
    WaterDefinition.cpp \
    SurfaceMaterial.cpp \
    CameraDefinition.cpp

HEADERS +=\
        definition_global.h \
    BaseDefinition.h \
    Layers.h \
    LegacyLayer.h \
    WaterDefinition.h \
    SurfaceMaterial.h \
    CameraDefinition.h

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../basics/release/ -lpaysages_basics
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../basics/debug/ -lpaysages_basics
else:unix: LIBS += -L$$OUT_PWD/../basics/ -lpaysages_basics
INCLUDEPATH += $$PWD/../basics
DEPENDPATH += $$PWD/../basics
