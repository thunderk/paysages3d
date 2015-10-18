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
    Layers.cpp \
    WaterDefinition.cpp \
    SurfaceMaterial.cpp \
    CameraDefinition.cpp \
    CloudsDefinition.cpp \
    CloudLayerDefinition.cpp \
    AtmosphereDefinition.cpp \
    TexturesDefinition.cpp \
    TextureLayerDefinition.cpp \
    Zone.cpp \
    TerrainDefinition.cpp \
    TerrainHeightMap.cpp \
    Scenery.cpp \
    VegetationLayerDefinition.cpp \
    VegetationDefinition.cpp \
    VegetationModelDefinition.cpp \
    VegetationInstance.cpp \
    PaintedGrid.cpp \
    PaintedGridBrush.cpp \
    PaintedGridData.cpp \
    DefinitionNode.cpp \
    FloatNode.cpp \
    DefinitionDiff.cpp \
    FloatDiff.cpp \
    DiffManager.cpp \
    DefinitionWatcher.cpp \
    IntNode.cpp \
    IntDiff.cpp \
    GodRaysDefinition.cpp \
    TimeManager.cpp \
    VegetationPresenceDefinition.cpp

HEADERS +=\
        definition_global.h \
    Layers.h \
    WaterDefinition.h \
    SurfaceMaterial.h \
    CameraDefinition.h \
    CloudsDefinition.h \
    CloudLayerDefinition.h \
    AtmosphereDefinition.h \
    TexturesDefinition.h \
    TextureLayerDefinition.h \
    Zone.h \
    TerrainDefinition.h \
    TerrainHeightMap.h \
    Scenery.h \
    VegetationLayerDefinition.h \
    VegetationDefinition.h \
    VegetationModelDefinition.h \
    VegetationInstance.h \
    PaintedGrid.h \
    PaintedGridBrush.h \
    PaintedGridData.h \
    DefinitionNode.h \
    FloatNode.h \
    DefinitionDiff.h \
    FloatDiff.h \
    DiffManager.h \
    DefinitionWatcher.h \
    IntNode.h \
    IntDiff.h \
    GodRaysDefinition.h \
    TimeManager.h \
    VegetationPresenceDefinition.h

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
