include(../common.pri)

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

TARGET = paysages-tests

OBJECTS_DIR = $$DESTDIR/testing/

linux-clang {
    CONFIG += link_pkgconfig
    PKGCONFIG += check
}

DEPENDPATH += ..
linux-clang:LIBS += -L$$DESTDIR -lpaysages_rendering

SOURCES += main.c \
    test_terrain_painting.c \
    test_render.c \
    test_pack.c \
    test_noise.c \
    test_euclid.c \
    test_clouds.c \
    test_camera.c \
    test_bruneton.c

HEADERS += \
    common.h

