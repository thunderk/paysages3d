#-------------------------------------------------
#
# Project created by QtCreator 2013-11-04T11:54:27
#
#-------------------------------------------------

QT       += opengl

TARGET = paysages_render_opengl
TEMPLATE = lib

DEFINES += OPENGL_LIBRARY

include(../../common.pri)

SOURCES += \
    OpenGLRenderer.cpp \
    BaseExplorerChunk.cpp \
    ExplorerChunkTerrain.cpp \
    WidgetExplorer.cpp \
    OpenGLShaderProgram.cpp \
    OpenGLPart.cpp \
    OpenGLSkybox.cpp \
    OpenGLWater.cpp \
    OpenGLSharedState.cpp \
    OpenGLVariable.cpp

HEADERS +=\
        opengl_global.h \
    OpenGLRenderer.h \
    BaseExplorerChunk.h \
    ExplorerChunkTerrain.h \
    WidgetExplorer.h \
    OpenGLShaderProgram.h \
    OpenGLPart.h \
    OpenGLSkybox.h \
    OpenGLWater.h \
    OpenGLSharedState.h \
    OpenGLVariable.h

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../software/release/ -lpaysages_render_software
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../software/debug/ -lpaysages_render_software
else:unix: LIBS += -L$$OUT_PWD/../software/ -lpaysages_render_software
INCLUDEPATH += $$PWD/../software
DEPENDPATH += $$PWD/../software

RESOURCES += \
    shaders/resources.qrc

OTHER_FILES += \
    shaders/skybox.frag \
    shaders/skybox.vert \
    shaders/water.vert \
    shaders/water.frag \
    shaders/bruneton.frag \
    shaders/bruneton.frag \
    shaders/tonemapping.frag
