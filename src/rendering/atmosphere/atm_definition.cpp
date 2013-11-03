#include "private.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "../tools.h"
#include "../renderer.h"

static int _inited = 0;

/******************** Definition ********************/
static void _validateDefinition(AtmosphereDefinition* definition)
{
    if (definition->hour < 0)
    {
        definition->hour = 0;
    }
    if (definition->hour > 23)
    {
        definition->hour = 23;
    }
    if (definition->minute < 0)
    {
        definition->minute = 0;
    }
    if (definition->minute > 59)
    {
        definition->minute = 59;
    }

    definition->_daytime = (double)definition->hour / 24.0 + (double)definition->minute / 1440.0;
}

static AtmosphereDefinition* _createDefinition()
{
    AtmosphereDefinition* result;

    /* TODO Find a better place for this */
    if (!_inited)
    {
        _inited = 1;
        brunetonInit();
    }

    result = new AtmosphereDefinition;

    atmosphereAutoPreset(result, ATMOSPHERE_PRESET_CLEAR_DAY);

    return result;
}

static void _deleteDefinition(AtmosphereDefinition* definition)
{
    delete definition;
}

static void _copyDefinition(AtmosphereDefinition* source, AtmosphereDefinition* destination)
{
    destination->model = source->model;
    destination->hour = source->hour;
    destination->minute = source->minute;
    destination->sun_color = source->sun_color;
    destination->sun_radius = source->sun_radius;
    destination->dome_lighting = source->dome_lighting;
    destination->humidity = source->humidity;

    _validateDefinition(destination);
}

static void _saveDefinition(PackStream* stream, AtmosphereDefinition* definition)
{
    stream->write((int*)&definition->model);
    stream->write(&definition->hour);
    stream->write(&definition->minute);
    colorSave(stream, &definition->sun_color);
    stream->write(&definition->sun_radius);
    stream->write(&definition->dome_lighting);
    stream->write(&definition->humidity);
}

static void _loadDefinition(PackStream* stream, AtmosphereDefinition* definition)
{
    stream->read((int*)&definition->model);
    stream->read(&definition->hour);
    stream->read(&definition->minute);
    colorLoad(stream, &definition->sun_color);
    stream->read(&definition->sun_radius);
    stream->read(&definition->dome_lighting);
    stream->read(&definition->humidity);

    _validateDefinition(definition);
}

StandardDefinition AtmosphereDefinitionClass = {
    (FuncObjectCreate)_createDefinition,
    (FuncObjectDelete)_deleteDefinition,
    (FuncObjectCopy)_copyDefinition,
    (FuncObjectValidate)_validateDefinition,
    (FuncObjectSave)_saveDefinition,
    (FuncObjectLoad)_loadDefinition
};
