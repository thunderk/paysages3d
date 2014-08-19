TEMPLATE = app

QT += qml quick widgets

include(../../common.pri)

SOURCES += main.cpp \
    OpenGLView.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    quick_global.h \
    OpenGLView.h

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../render/software/release/ -lpaysages_render_software
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../render/software/debug/ -lpaysages_render_software
else:unix: LIBS += -L$$OUT_PWD/../../render/software/ -lpaysages_render_software
INCLUDEPATH += $$PWD/../../render/software
DEPENDPATH += $$PWD/../../render/software

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../render/opengl/release/ -lpaysages_render_opengl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../render/opengl/debug/ -lpaysages_render_opengl
else:unix: LIBS += -L$$OUT_PWD/../../render/opengl/ -lpaysages_render_opengl
INCLUDEPATH += $$PWD/../../render/opengl
DEPENDPATH += $$PWD/../../render/opengl
