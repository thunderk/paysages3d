TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

TARGET = paysages-tests

CONFIG(release, debug|release): DEFINES += NDEBUG

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += check
}

INCLUDEPATH += $$PWD/..

SOURCES += main.c \
    test_terrain_painting.c \
    test_render.c \
    test_pack.c \
    test_noise.c \
    test_euclid.c \
    test_clouds.c \
    test_camera.c \
    test_bruneton.c \
    test_zone.c

HEADERS += \
    common.h

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
