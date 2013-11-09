#-------------------------------------------------
#
# Project created by QtCreator 2013-11-09T15:13:23
#
#-------------------------------------------------

QT       -= gui

TARGET = paysages_render_software
TEMPLATE = lib

DEFINES += SOFTWARE_LIBRARY

include(../../common.pri)

SOURCES += SoftwareRenderer.cpp \
    global/FluidMediumTraversal.cpp

HEADERS += SoftwareRenderer.h\
        software_global.h \
    global/FluidMediumTraversal.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../definition/release/ -lpaysages_definition
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../definition/debug/ -lpaysages_definition
else:unix: LIBS += -L$$OUT_PWD/../../definition/ -lpaysages_definition
INCLUDEPATH += $$PWD/../../definition
DEPENDPATH += $$PWD/../../definition

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../rendering/release/ -lpaysages_rendering
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../rendering/debug/ -lpaysages_rendering
else:unix: LIBS += -L$$OUT_PWD/../../rendering/ -lpaysages_rendering
INCLUDEPATH += $$PWD/../../rendering
DEPENDPATH += $$PWD/../../rendering
