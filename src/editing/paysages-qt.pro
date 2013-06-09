isEmpty(PROJECT_PATH) {
PROJECT_PATH = ../..
}

TEMPLATE = app
CONFIG += qt
QT += core gui opengl
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = $$PROJECT_PATH/build/$$BUILDMODE/
OBJECTS_DIR = $$DESTDIR/editing/
MOC_DIR = $$OBJECTS_DIR/moc/
CONFIG -= release
CONFIG += $$BUILDMODE
INCLUDEPATH += $$PROJECT_PATH/src/

release:DEFINES += NDEBUG
release:QMAKE_CXXFLAGS += -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-variable

LIBS += -lGLU
unix:LIBS += -L$$DESTDIR -lpaysages_rendering -lpaysages_exploring
win32:LIBS += ../libpaysages.a ../libpaysages_exploring.a -lDevIL -lILU -lILUT -lglib-2.0 -lgthread-2.0

TRANSLATIONS = $$PROJECT_PATH/data/i18n/paysages_fr.ts

HEADERS += \
    terrain/widgetheightmap.h \
    widgetexplorer.h \
    widgetcurveeditor.h \
    tools.h \
    previewosd.h \
    previewmaterial.h \
    previewcolorgradation.h \
    inputnoise.h \
    inputmaterial.h \
    inputlayers.h \
    inputint.h \
    inputenum.h \
    inputdouble.h \
    inputcurve.h \
    inputcolorgradation.h \
    inputcolor.h \
    inputcamera.h \
    inputboolean.h \
    formwater.h \
    formtextures.h \
    formrender.h \
    formmaterial.h \
    formclouds.h \
    formatmosphere.h \
    explorerchunkterrain.h \
    explorerchunksky.h \
    dialogrender.h \
    dialognoise.h \
    dialogmaterial.h \
    dialoglayers.h \
    dialogexplorer.h \
    dialogcurve.h \
    dialogcolorgradation.h \
    basepreview.h \
    baseinput.h \
    baseformlayer.h \
    baseform.h \
    baseexplorerchunk.h \
    terrain/dialogterrainpainting.h \
    common/widgetglobalformbuttons.h \
    terrain/paintingbrush.h \
    terrain/mainterrainform.h \
    common/freeformhelper.h \
    terrain/previewterrainshape.h \
    common/widgetsliderdecimal.h \
    common/previewrenderer.h \
    terrain/widgetterrainbasenoisepreview.h \
    common/mainwindow.h

SOURCES += \
    terrain/widgetheightmap.cpp \
    widgetexplorer.cpp \
    widgetcurveeditor.cpp \
    tools.cpp \
    previewosd.cpp \
    previewmaterial.cpp \
    previewcolorgradation.cpp \
    inputnoise.cpp \
    inputmaterial.cpp \
    inputlayers.cpp \
    inputint.cpp \
    inputenum.cpp \
    inputdouble.cpp \
    inputcurve.cpp \
    inputcolorgradation.cpp \
    inputcolor.cpp \
    inputcamera.cpp \
    inputboolean.cpp \
    formwater.cpp \
    formtextures.cpp \
    formrender.cpp \
    formmaterial.cpp \
    formclouds.cpp \
    formatmosphere.cpp \
    explorerchunkterrain.cpp \
    explorerchunksky.cpp \
    dialogrender.cpp \
    dialognoise.cpp \
    dialogmaterial.cpp \
    dialoglayers.cpp \
    dialogexplorer.cpp \
    dialogcurve.cpp \
    dialogcolorgradation.cpp \
    basepreview.cpp \
    baseinput.cpp \
    baseformlayer.cpp \
    baseform.cpp \
    baseexplorerchunk.cpp \
    terrain/dialogterrainpainting.cpp \
    common/widgetglobalformbuttons.cpp \
    terrain/paintingbrush.cpp \
    terrain/mainterrainform.cpp \
    common/freeformhelper.cpp \
    terrain/previewterrainshape.cpp \
    common/widgetsliderdecimal.cpp \
    common/previewrenderer.cpp \
    terrain/widgetterrainbasenoisepreview.cpp \
    common/mainwindow.cpp

FORMS += \
    terrain/dialogterrainpainting.ui \
    common/widgetglobalformbuttons.ui \
    terrain/mainterrainform.ui \
    common/mainwindow.ui

RESOURCES += \
    ../../data/ui_pictures.qrc
