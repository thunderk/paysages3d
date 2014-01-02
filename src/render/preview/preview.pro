#-------------------------------------------------
#
# Project created by QtCreator 2013-11-11T17:56:07
#
#-------------------------------------------------

TARGET = paysages_render_preview
TEMPLATE = lib
QT += gui widgets

DEFINES += PREVIEW_LIBRARY

include(../../common.pri)

SOURCES += Base2dPreviewRenderer.cpp \
    AtmosphereColorPreviewRenderer.cpp \
    BasePreview.cpp \
    PreviewOsd.cpp \
    DrawingWidget.cpp \
    PreviewOsdItem.cpp \
    CloudsCoveragePreviewRenderer.cpp \
    CloudsAspectPreviewRenderer.cpp \
    WaterCoveragePreviewRenderer.cpp \
    WaterAspectPreviewRenderer.cpp \
    TerrainShapePreviewRenderer.cpp \
    SceneryTopDownPreviewRenderer.cpp \
    TexturesMixPreviewRenderer.cpp \
    TextureLayerCoveragePreviewRenderer.cpp \
    TextureLayerAspectPreviewRenderer.cpp

HEADERS += Base2dPreviewRenderer.h\
        preview_global.h \
    AtmosphereColorPreviewRenderer.h \
    BasePreview.h \
    PreviewOsd.h \
    DrawingWidget.h \
    PreviewOsdItem.h \
    CloudsCoveragePreviewRenderer.h \
    CloudsAspectPreviewRenderer.h \
    WaterCoveragePreviewRenderer.h \
    WaterAspectPreviewRenderer.h \
    TerrainShapePreviewRenderer.h \
    SceneryTopDownPreviewRenderer.h \
    TexturesMixPreviewRenderer.h \
    TextureLayerCoveragePreviewRenderer.h \
    TextureLayerAspectPreviewRenderer.h

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
