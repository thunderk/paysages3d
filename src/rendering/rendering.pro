CONFIG += console
CONFIG -= app_bundle

TEMPLATE = lib
TARGET = paysages_rendering

DEFINES += RENDERING_LIBRARY

include(../common.pri)

INCLUDEPATH += $$PWD/..

SOURCES += main.cpp \
    tools.cpp \
    renderer.cpp \
    render.cpp \
    opencl.cpp \
    geoarea.cpp \
    camera.cpp \
    atmosphere/atm_render.cpp \
    atmosphere/atm_raster.cpp \
    atmosphere/atm_preview.cpp \
    atmosphere/atm_presets.cpp \
    atmosphere/atm_definition.cpp \
    atmosphere/atm_bruneton.cpp \
    clouds/clo_walking.cpp \
    clouds/clo_rendering.cpp \
    clouds/clo_preview.cpp \
    clouds/clo_presets.cpp \
    clouds/clo_density.cpp \
    clouds/clo_definition.cpp \
    shared/preview.cpp \
    terrain/ter_render.cpp \
    terrain/ter_raster.cpp \
    terrain/ter_preview.cpp \
    terrain/ter_presets.cpp \
    terrain/ter_painting.cpp \
    terrain/ter_definition.cpp \
    textures/tex_tools.cpp \
    textures/tex_rendering.cpp \
    textures/tex_preview.cpp \
    textures/tex_presets.cpp \
    textures/tex_definition.cpp \
    tools/zone.cpp \
    tools/texture.cpp \
    tools/parallel.cpp \
    tools/memory.cpp \
    tools/lighting.cpp \
    tools/euclid.cpp \
    tools/data.cpp \
    tools/curve.cpp \
    tools/color.cpp \
    tools/cache.cpp \
    tools/boundingbox.cpp \
    tools/array.cpp \
    water/wat_render.cpp \
    water/wat_raster.cpp \
    water/wat_preview.cpp \
    water/wat_presets.cpp \
    water/wat_definition.cpp \
    Scenery.cpp

HEADERS += \
    tools.h \
    renderer.h \
    render.h \
    opencl.h \
    main.h \
    geoarea.h \
    camera.h \
    atmosphere/public.h \
    atmosphere/private.h \
    clouds/public.h \
    clouds/private.h \
    clouds/clo_walking.h \
    clouds/clo_preview.h \
    clouds/clo_density.h \
    shared/types.h \
    shared/preview.h \
    terrain/ter_raster.h \
    terrain/public.h \
    terrain/private.h \
    textures/tex_preview.h \
    textures/public.h \
    textures/private.h \
    tools/zone.h \
    tools/texture.h \
    tools/parallel.h \
    tools/memory.h \
    tools/lighting.h \
    tools/euclid.h \
    tools/data.h \
    tools/curve.h \
    tools/color.h \
    tools/cache.h \
    tools/boundingbox.h \
    tools/array.h \
    water/public.h \
    water/private.h \
    rendering_global.h \
    Scenery.h

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
