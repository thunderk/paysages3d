#include "private.h"

#include <stdlib.h>

static void _validateDefinition(WaterDefinition* definition)
{
    double scaling = definition->scaling * 0.3;
    noiseClearLevels(definition->_waves_noise);
    if (definition->waves_height > 0.0)
    {
        noiseAddLevelsSimple(definition->_waves_noise, 2, scaling, -definition->waves_height * scaling * 0.015, definition->waves_height * scaling * 0.015, 0.5);
    }
    if (definition->detail_height > 0.0)
    {
        noiseAddLevelsSimple(definition->_waves_noise, 3, scaling * 0.1, -definition->detail_height * scaling * 0.015, definition->detail_height * scaling * 0.015, 0.5);
    }
    noiseSetFunctionParams(definition->_waves_noise, NOISE_FUNCTION_SIMPLEX, -definition->turbulence, 0.0);
    noiseValidate(definition->_waves_noise);

    materialValidate(&definition->material);
    materialValidate(&definition->foam_material);
}

static WaterDefinition* _createDefinition()
{
    WaterDefinition* definition = new WaterDefinition;

    definition->_waves_noise = noiseCreateGenerator();

    waterAutoPreset(definition, WATER_PRESET_LAKE);

    return definition;
}

static void _deleteDefinition(WaterDefinition* definition)
{
    noiseDeleteGenerator(definition->_waves_noise);
    delete definition;
}

static void _copyDefinition(WaterDefinition* source, WaterDefinition* destination)
{
    NoiseGenerator* noise;

    noise = destination->_waves_noise;
    *destination = *source;
    destination->_waves_noise = noise;
    noiseCopy(source->_waves_noise, destination->_waves_noise);
}

static void _saveDefinition(PackStream* stream, WaterDefinition* definition)
{
    materialSave(stream, &definition->material);
    colorSave(stream, &definition->depth_color);
    packWriteDouble(stream, &definition->transparency_depth);
    packWriteDouble(stream, &definition->transparency);
    packWriteDouble(stream, &definition->reflection);
    packWriteDouble(stream, &definition->lighting_depth);

    packWriteDouble(stream, &definition->scaling);
    packWriteDouble(stream, &definition->waves_height);
    packWriteDouble(stream, &definition->detail_height);
    packWriteDouble(stream, &definition->turbulence);

    packWriteDouble(stream, &definition->foam_coverage);
    materialSave(stream, &definition->foam_material);

    noiseSaveGenerator(stream, definition->_waves_noise);
}

static void _loadDefinition(PackStream* stream, WaterDefinition* definition)
{
    materialLoad(stream, &definition->material);
    colorLoad(stream, &definition->depth_color);
    packReadDouble(stream, &definition->transparency_depth);
    packReadDouble(stream, &definition->transparency);
    packReadDouble(stream, &definition->reflection);
    packReadDouble(stream, &definition->lighting_depth);

    packReadDouble(stream, &definition->scaling);
    packReadDouble(stream, &definition->waves_height);
    packReadDouble(stream, &definition->detail_height);
    packReadDouble(stream, &definition->turbulence);

    packReadDouble(stream, &definition->foam_coverage);
    materialLoad(stream, &definition->foam_material);

    noiseLoadGenerator(stream, definition->_waves_noise);

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
