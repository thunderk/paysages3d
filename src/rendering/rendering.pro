#-------------------------------------------------
#
# Project created by QtCreator 2013-05-01T12:27:33
#
#-------------------------------------------------

QT       -= core gui

TARGET = rendering
TEMPLATE = lib

DEFINES += RENDERING_LIBRARY

SOURCES += \
    zone.c \
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
    main.c \
    layers.c \
    geoarea.c \
    camera.c \
    auto.c \
    atmosphere/atm_render.c \
    atmosphere/atm_raster.c \
    atmosphere/atm_preview.c \
    atmosphere/atm_presets.c \
    atmosphere/atm_definition.c \
    atmosphere/atm_bruneton.c \
    clouds/clo_tools.c \
    clouds/clo_rendering.c \
    clouds/clo_preview.c \
    clouds/clo_presets.c \
    clouds/clo_definition.c \
    shared/preview.c \
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
    tools/texture.c \
    tools/parallel.c \
    tools/pack.c \
    tools/memory.c \
    tools/lighting.c \
    tools/euclid.c \
    tools/curve.c \
    tools/color.c \
    tools/cache.c \
    tools/array.c \
    water/wat_render.c \
    water/wat_raster.c \
    water/wat_preview.c \
    water/wat_presets.c \
    water/wat_definition.c

HEADERS += \
    zone.h \
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
    auto.h \
    atmosphere/public.h \
    atmosphere/private.h \
    clouds/public.h \
    clouds/private.h \
    shared/types.h \
    shared/preview.h \
    terrain/public.h \
    terrain/private.h \
    textures/public.h \
    textures/private.h \
    tools/texture.h \
    tools/parallel.h \
    tools/pack.h \
    tools/memory.h \
    tools/lighting.h \
    tools/euclid.h \
    tools/curve.h \
    tools/color.h \
    tools/cache.h \
    tools/array.h \
    water/public.h \
    water/private.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xEC48CBFE
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = rendering.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
