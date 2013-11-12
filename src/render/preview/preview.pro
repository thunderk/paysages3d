#-------------------------------------------------
#
# Project created by QtCreator 2013-11-11T17:56:07
#
#-------------------------------------------------

TARGET = paysages_render_preview
TEMPLATE = lib

DEFINES += PREVIEW_LIBRARY

include(../../common.pri)

SOURCES += Base2dPreviewRenderer.cpp \
    AtmosphereColorPreviewRenderer.cpp

HEADERS += Base2dPreviewRenderer.h\
        preview_global.h \
    AtmosphereColorPreviewRenderer.h

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../software/release/ -lpaysages_render_software
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../software/debug/ -lpaysages_render_software
else:unix: LIBS += -L$$OUT_PWD/../software/ -lpaysages_render_software
INCLUDEPATH += $$PWD/../software
DEPENDPATH += $$PWD/../software
