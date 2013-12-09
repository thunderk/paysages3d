#include "RenderingScenery.h"

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
    Scenery::save(stream);

    if (_custom_save)
    {
        _custom_save(stream, _custom_data);
    }
}

void RenderingScenery::load(PackStream* stream)
{
    Scenery::load(stream);

    if (_custom_load)
    {
        _custom_load(stream, _custom_data);
    }
}
