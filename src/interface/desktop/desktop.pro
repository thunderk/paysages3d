TEMPLATE = app
CONFIG += qt

QT += core gui opengl

TARGET = paysages-gui

unix:LIBS += -lGLU

include(../../common.pri)

HEADERS += \
    terrain/widgetheightmap.h \
    widgetcurveeditor.h \
    tools.h \
    previewmaterial.h \
    inputnoise.h \
    inputmaterial.h \
    inputlayers.h \
    inputint.h \
    inputenum.h \
    inputdouble.h \
    inputcurve.h \
    inputcolor.h \
    inputcamera.h \
    inputboolean.h \
    formwater.h \
    formtextures.h \
    formrender.h \
    formclouds.h \
    formatmosphere.h \
    dialogrender.h \
    dialognoise.h \
    dialoglayers.h \
    dialogexplorer.h \
    dialogcurve.h \
    baseinput.h \
    baseformlayer.h \
    baseform.h \
    terrain/dialogterrainpainting.h \
    common/widgetglobalformbuttons.h \
    terrain/paintingbrush.h \
    terrain/mainterrainform.h \
    common/freeformhelper.h \
    terrain/previewterrainshape.h \
    common/widgetsliderdecimal.h \
    terrain/widgetterrainbasenoisepreview.h \
    common/mainwindow.h \
    terrain/dialogbaseterrainnoise.h \
    textures/maintexturesform.h \
    common/freelayerhelper.h \
    lighting/DialogMaterialEditor.h \
    lighting/SmallPreviewColor.h \
    lighting/SmallPreviewHues.h \
    textures/PreviewLayerCoverage.h \
    textures/PreviewLayerLook.h \
    textures/PreviewCumul.h \
    textures/DialogTexturesLayer.h \
    desktop_global.h

SOURCES += \
    terrain/widgetheightmap.cpp \
    widgetcurveeditor.cpp \
    tools.cpp \
    previewmaterial.cpp \
    inputnoise.cpp \
    inputmaterial.cpp \
    inputlayers.cpp \
    inputint.cpp \
    inputenum.cpp \
    inputdouble.cpp \
    inputcurve.cpp \
    inputcolor.cpp \
    inputcamera.cpp \
    inputboolean.cpp \
    formwater.cpp \
    formtextures.cpp \
    formrender.cpp \
    formclouds.cpp \
    formatmosphere.cpp \
    dialogrender.cpp \
    dialognoise.cpp \
    dialoglayers.cpp \
    dialogexplorer.cpp \
    dialogcurve.cpp \
    baseinput.cpp \
    baseformlayer.cpp \
    baseform.cpp \
    terrain/dialogterrainpainting.cpp \
    common/widgetglobalformbuttons.cpp \
    terrain/paintingbrush.cpp \
    terrain/mainterrainform.cpp \
    common/freeformhelper.cpp \
    terrain/previewterrainshape.cpp \
    common/widgetsliderdecimal.cpp \
    terrain/widgetterrainbasenoisepreview.cpp \
    common/mainwindow.cpp \
    terrain/dialogbaseterrainnoise.cpp \
    textures/maintexturesform.cpp \
    common/freelayerhelper.cpp \
    lighting/DialogMaterialEditor.cpp \
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
    ../../../data/ui_pictures.qrc

TRANSLATIONS = ../../../data/i18n/paysages_fr.ts

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../rendering/release/ -lpaysages_rendering
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../rendering/debug/ -lpaysages_rendering
else:unix: LIBS += -L$$OUT_PWD/../../rendering/ -lpaysages_rendering
INCLUDEPATH += $$PWD/../../rendering
DEPENDPATH += $$PWD/../../rendering

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../definition/release/ -lpaysages_definition
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../definition/debug/ -lpaysages_definition
else:unix: LIBS += -L$$OUT_PWD/../../definition/ -lpaysages_definition
INCLUDEPATH += $$PWD/../../definition
DEPENDPATH += $$PWD/../../definition

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../render/software/release/ -lpaysages_render_software
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../render/software/debug/ -lpaysages_render_software
else:unix: LIBS += -L$$OUT_PWD/../../render/software/ -lpaysages_render_software
INCLUDEPATH += $$PWD/../../render/software
DEPENDPATH += $$PWD/../../render/software

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../render/preview/release/ -lpaysages_render_preview
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../render/preview/debug/ -lpaysages_render_preview
else:unix: LIBS += -L$$OUT_PWD/../../render/preview/ -lpaysages_render_preview
INCLUDEPATH += $$PWD/../../render/preview
DEPENDPATH += $$PWD/../../render/preview

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../render/opengl/release/ -lpaysages_render_opengl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../render/opengl/debug/ -lpaysages_render_opengl
else:unix: LIBS += -L$$OUT_PWD/../../render/opengl/ -lpaysages_render_opengl
INCLUDEPATH += $$PWD/../../render/opengl
DEPENDPATH += $$PWD/../../render/opengl
