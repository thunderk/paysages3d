TEMPLATE = app

QT += qml quick widgets

include(../../../common.pri)

SOURCES += main.cpp \
    OpenGLView.cpp \
    MainModelerWindow.cpp \
    WaterModeler.cpp \
    AtmosphereModeler.cpp \
    RenderPreviewProvider.cpp \
    RenderProcess.cpp

RESOURCES += \
    qml/app.qrc

TARGET = paysages-modeler

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = ../extension

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    OpenGLView.h \
    modeler_global.h \
    MainModelerWindow.h \
    WaterModeler.h \
    AtmosphereModeler.h \
    RenderPreviewProvider.h \
    RenderProcess.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../system/release/ -lpaysages_system
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../system/debug/ -lpaysages_system
else:unix: LIBS += -L$$OUT_PWD/../../../system/ -lpaysages_system
INCLUDEPATH += $$PWD/../../../system
DEPENDPATH += $$PWD/../../../system

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../basics/release/ -lpaysages_basics
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../basics/debug/ -lpaysages_basics
else:unix: LIBS += -L$$OUT_PWD/../../../basics/ -lpaysages_basics
INCLUDEPATH += $$PWD/../../../basics
DEPENDPATH += $$PWD/../../../basics

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../definition/release/ -lpaysages_definition
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../definition/debug/ -lpaysages_definition
else:unix: LIBS += -L$$OUT_PWD/../../../definition/ -lpaysages_definition
INCLUDEPATH += $$PWD/../../../definition
DEPENDPATH += $$PWD/../../../definition

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../render/software/release/ -lpaysages_render_software
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../render/software/debug/ -lpaysages_render_software
else:unix: LIBS += -L$$OUT_PWD/../../../render/software/ -lpaysages_render_software
INCLUDEPATH += $$PWD/../../../render/software
DEPENDPATH += $$PWD/../../../render/software

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../render/opengl/release/ -lpaysages_render_opengl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../render/opengl/debug/ -lpaysages_render_opengl
else:unix: LIBS += -L$$OUT_PWD/../../../render/opengl/ -lpaysages_render_opengl
INCLUDEPATH += $$PWD/../../../render/opengl
DEPENDPATH += $$PWD/../../../render/opengl

OTHER_FILES += \
    qml/main.qml \
    qml/ToolbarButton.qml \
    qml/OpenGLView.qml \
    qml/Toolbar.qml \
    qml/Tooltip.qml \
    qml/BasePanel.qml \
    qml/PanelWaterLevel.qml \
    qml/PanelAtmosphereDaytime.qml \
    qml/BaseSlider.qml \
    qml/BaseChoice.qml \
    qml/BaseChoiceItem.qml \
    qml/RenderDialog.qml
