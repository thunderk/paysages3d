#include "private.h"

#include "PackStream.h"
#include "NoiseGenerator.h"

static void _validateDefinition(WaterDefinition* definition)
{
    double scaling = definition->scaling * 0.3;
    definition->_waves_noise->clearLevels();
    if (definition->waves_height > 0.0)
    {
        definition->_waves_noise->addLevelsSimple(2, scaling, -definition->waves_height * scaling * 0.015, definition->waves_height * scaling * 0.015, 0.5);
    }
    if (definition->detail_height > 0.0)
    {
        definition->_waves_noise->addLevelsSimple(3, scaling * 0.1, -definition->detail_height * scaling * 0.015, definition->detail_height * scaling * 0.015, 0.5);
    }
    definition->_waves_noise->setFunctionParams(NOISE_FUNCTION_SIMPLEX, -definition->turbulence, 0.0);
    definition->_waves_noise->validate();

    materialValidate(&definition->material);
    materialValidate(&definition->foam_material);
}

static WaterDefinition* _createDefinition()
{
    WaterDefinition* definition = new WaterDefinition;

    definition->_waves_noise = new NoiseGenerator();

    waterAutoPreset(definition, WATER_PRESET_LAKE);

    return definition;
}

static void _deleteDefinition(WaterDefinition* definition)
{
    delete definition->_waves_noise;
    delete definition;
}

static void _copyDefinition(WaterDefinition* source, WaterDefinition* destination)
{
    NoiseGenerator* noise;

    noise = destination->_waves_noise;
    *destination = *source;
    destination->_waves_noise = noise;
    source->_waves_noise->copy(destination->_waves_noise);
}

static void _saveDefinition(PackStream* stream, WaterDefinition* definition)
{
    materialSave(stream, &definition->material);
    colorSave(stream, &definition->depth_color);
    stream->write(&definition->transparency_depth);
    stream->write(&definition->transparency);
    stream->write(&definition->reflection);
    stream->write(&definition->lighting_depth);

    stream->write(&definition->scaling);
    stream->write(&definition->waves_height);
    stream->write(&definition->detail_height);
    stream->write(&definition->turbulence);

    stream->write(&definition->foam_coverage);
    materialSave(stream, &definition->foam_material);

    definition->_waves_noise->save(stream);
}

static void _loadDefinition(PackStream* stream, WaterDefinition* definition)
{
    materialLoad(stream, &definition->material);
    colorLoad(stream, &definition->depth_color);
    stream->read(&definition->transparency_depth);
    stream->read(&definition->transparency);
    stream->read(&definition->reflection);
    stream->read(&definition->lighting_depth);

    stream->read(&definition->scaling);
    stream->read(&definition->waves_height);
    stream->read(&definition->detail_height);
    stream->read(&definition->turbulence);

    stream->read(&definition->foam_coverage);
    materialLoad(stream, &definition->foam_material);

    definition->_waves_noise->load(stream);

    _validateDefinition(definition);
}

StandardDefinition WaterDefinitionClass = {
    (FuncObjectCreate)_createDefinition,
    (FuncObjectDelete)_deleteDefinition,
    (FuncObjectCopy)_copyDefinition,
    (FuncObjectValidate)_validateDefinition,
    (FuncObjectSave)_saveDefinition,
    (FuncObjectLoad)_loadDefinition
};
