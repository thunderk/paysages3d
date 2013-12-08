#include "RenderingScenery.h"

#include "renderer.h"
#include "terrain/public.h"
#include "textures/public.h"
#include "water/public.h"
#include "atmosphere/public.h"
#include "CameraDefinition.h"

static RenderingScenery _main_scenery;

RenderingScenery::RenderingScenery()
{
    _custom_load = NULL;
    _custom_save = NULL;
    _custom_data = NULL;
}

RenderingScenery* RenderingScenery::getCurrent()
{
    return &_main_scenery;
}

void RenderingScenery::setCustomSaveCallbacks(SceneryCustomDataCallback callback_save, SceneryCustomDataCallback callback_load, void* data)
{
    _custom_save = callback_save;
    _custom_load = callback_load;
    _custom_data = data;
}

void RenderingScenery::save(PackStream* stream) const
{
    if (_custom_save)
    {
        _custom_save(stream, _custom_data);
    }

    Scenery::save(stream);
}

void RenderingScenery::load(PackStream* stream)
{
    if (_custom_load)
    {
        _custom_load(stream, _custom_data);
    }

    Scenery::load(stream);
}


void RenderingScenery::bindToRenderer(Renderer* renderer)
{
    renderer->setScenery(this);
    renderer->prepare();
}
