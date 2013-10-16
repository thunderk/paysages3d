TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

TARGET = paysages-tests

CONFIG(release, debug|release): DEFINES += NDEBUG

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += check
}

INCLUDEPATH += $$PWD/..

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../rendering/release/ -lpaysages_rendering
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../rendering/debug/ -lpaysages_rendering
else:unix: LIBS += -L$$OUT_PWD/../rendering/ -lpaysages_rendering
DEPENDPATH += $$PWD/../rendering

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
