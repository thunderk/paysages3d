TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

TARGET = paysages-tests

include(../common.pri)

SOURCES += main.cpp \
    Layers_Test.cpp \
    PackStream_Test.cpp \
    NoiseGenerator_Test.cpp \
    Render_Test.cpp \
    TerrainPainting_Test.cpp \
    Zone_Test.cpp \
    Euclid_Test.cpp \
    Bruneton_Test.cpp \
    Camera_Test.cpp \
    Clouds_Test.cpp \
    FluidMediumManager_Test.cpp

HEADERS += \
    BaseTestCase.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/googletest/release/ -lgoogletest
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/googletest/debug/ -lgoogletest
else:unix: LIBS += -L$$OUT_PWD/googletest/ -lgoogletest
INCLUDEPATH += $$PWD/googletest/include
DEPENDPATH += $$PWD/googletest/include

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../definition/release/ -lpaysages_definition
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../definition/debug/ -lpaysages_definition
else:unix: LIBS += -L$$OUT_PWD/../definition/ -lpaysages_definition
INCLUDEPATH += $$PWD/../definition
DEPENDPATH += $$PWD/../definition

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../rendering/release/ -lpaysages_rendering
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../rendering/debug/ -lpaysages_rendering
else:unix: LIBS += -L$$OUT_PWD/../rendering/ -lpaysages_rendering
INCLUDEPATH += $$PWD/../rendering
DEPENDPATH += $$PWD/../rendering

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../render/software/release/ -lpaysages_render_software
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../render/software/debug/ -lpaysages_render_software
else:unix: LIBS += -L$$OUT_PWD/../render/software/ -lpaysages_render_software
INCLUDEPATH += $$PWD/../render/software
DEPENDPATH += $$PWD/../render/software
