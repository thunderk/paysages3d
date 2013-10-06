TEMPLATE = lib
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

TARGET = paysages_rendering

INCLUDEPATH += $$PWD/..

linux-clang {
    CONFIG += link_pkgconfig
    PKGCONFIG += glib-2.0 gthread-2.0 IL ILU
}
DEFINES += HAVE_GLIB=1
CONFIG(release, debug|release): DEFINES += NDEBUG

SOURCES += main.c \
    tools.c \
    system.c \
    scenery.c \
    renderer.c \
    render.c \
    opencl.c \
    noisesimplex.c \
    noiseperlin.c \
    noisenaive.c \
    noise.c \
    layers.c \
    geoarea.c \
    camera.c \
    atmosphere/atm_render.c \
    atmosphere/atm_raster.c \
    atmosphere/atm_preview.c \
    atmosphere/atm_presets.c \
    atmosphere/atm_definition.c \
    atmosphere/atm_bruneton.c \
    clouds/clo_walking.c \
    clouds/clo_rendering.c \
    clouds/clo_preview.c \
    clouds/clo_presets.c \
    clouds/clo_density.c \
    clouds/clo_definition.c \
    shared/preview.c \
    terrain/ter_render.c \
    terrain/ter_raster.c \
    terrain/ter_preview.c \
    terrain/ter_presets.c \
    terrain/ter_painting.c \
    terrain/ter_definition.c \
    textures/tex_tools.c \
    textures/tex_rendering.c \
    textures/tex_preview.c \
    textures/tex_presets.c \
    textures/tex_definition.c \
    tools/zone.c \
    tools/texture.c \
    tools/parallel.c \
    tools/pack.c \
    tools/memory.c \
    tools/lighting.c \
    tools/euclid.c \
    tools/data.c \
    tools/curve.c \
    tools/color.c \
    tools/cache.c \
    tools/boundingbox.c \
    tools/array.c \
    water/wat_render.c \
    water/wat_raster.c \
    water/wat_preview.c \
    water/wat_presets.c \
    water/wat_definition.c

HEADERS += \
    tools.h \
    system.h \
    scenery.h \
    renderer.h \
    render.h \
    opencl.h \
    noisesimplex.h \
    noiseperlin.h \
    noisenaive.h \
    noise.h \
    main.h \
    layers.h \
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
    tools/pack.h \
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
    water/private.h

