TEMPLATE = app
CONFIG += qt

QT += core gui opengl

TARGET = paysages-gui

linux-clang:LIBS += -lGLU
win32:LIBS += ../../libpaysages.a -lDevIL -lILU -lILUT -lglib-2.0 -lgthread-2.0

CONFIG(release, debug|release): DEFINES += NDEBUG

INCLUDEPATH += $$PWD/..

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../exploring/release/ -lpaysages_exploring
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../exploring/debug/ -lpaysages_exploring
else:unix: LIBS += -L$$OUT_PWD/../exploring/ -lpaysages_exploring
DEPENDPATH += $$PWD/../exploring

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../rendering/release/ -lpaysages_rendering
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../rendering/debug/ -lpaysages_rendering
else:unix: LIBS += -L$$OUT_PWD/../rendering/ -lpaysages_rendering
DEPENDPATH += $$PWD/../rendering

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
    formclouds.h \
    formatmosphere.h \
    explorerchunkterrain.h \
    explorerchunksky.h \
    dialogrender.h \
    dialognoise.h \
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
    common/mainwindow.h \
    terrain/dialogbaseterrainnoise.h \
    textures/maintexturesform.h \
    common/freelayerhelper.h \
    lighting/DialogMaterialEditor.h \
    common/DrawingWidget.h \
    lighting/SmallPreviewColor.h \
    lighting/SmallPreviewHues.h \
    textures/PreviewLayerCoverage.h \
    textures/PreviewLayerLook.h \
    textures/PreviewCumul.h \
    textures/DialogTexturesLayer.h

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
    formclouds.cpp \
    formatmosphere.cpp \
    explorerchunkterrain.cpp \
    explorerchunksky.cpp \
    dialogrender.cpp \
    dialognoise.cpp \
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
    common/mainwindow.cpp \
    terrain/dialogbaseterrainnoise.cpp \
    textures/maintexturesform.cpp \
    common/freelayerhelper.cpp \
    lighting/DialogMaterialEditor.cpp \
    common/DrawingWidget.cpp \
    lighting/SmallPreviewColor.cpp \
    lighting/SmallPreviewHues.cpp \
    textures/PreviewLayerCoverage.cpp \
    textures/PreviewLayerLook.cpp \
    textures/PreviewCumul.cpp \
    textures/DialogTexturesLayer.cpp

FORMS += \
    terrain/dialogterrainpainting.ui \
    common/widgetglobalformbuttons.ui \
    terrain/mainterrainform.ui \
    common/mainwindow.ui \
    terrain/dialogbaseterrainnoise.ui \
    textures/maintexturesform.ui \
    lighting/DialogMaterialEditor.ui \
    textures/DialogTexturesLayer.ui

RESOURCES += \
    ../../data/ui_pictures.qrc

TRANSLATIONS = ../../data/i18n/paysages_fr.ts
