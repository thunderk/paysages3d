#include "private.h"

#include "../tools.h"
#include "../renderer.h"
#include "NoiseGenerator.h"
#include "PackStream.h"

/******************** Definition ********************/
static void _validateDefinition(TerrainDefinition* definition)
{
    definition->_height_noise->validate();

    if (definition->height < 1.0)
    {
        definition->height = 1.0;
    }

    /* Get minimal and maximal height */
    definition->_height_noise->getRange(&definition->_min_height, &definition->_max_height);
    definition->_min_height *= definition->height * definition->scaling;
    definition->_max_height *= definition->height * definition->scaling;

    /* TODO Alter with heightmap min/max */
}

static TerrainDefinition* _createDefinition()
{
    TerrainDefinition* definition = new TerrainDefinition;

    definition->height = 1.0;
    definition->scaling = 1.0;
    definition->shadow_smoothing = 0.0;

    definition->height_map = terrainHeightMapCreate(definition);

    definition->water_height = -0.3;

    definition->_height_noise = new NoiseGenerator();

    terrainAutoPreset(definition, TERRAIN_PRESET_STANDARD);

    return definition;
}

static void _deleteDefinition(TerrainDefinition* definition)
{
    terrainHeightmapDelete(definition->height_map);
    delete definition->_height_noise;
    delete definition;
}

static void _copyDefinition(TerrainDefinition* source, TerrainDefinition* destination)
{
    destination->height = source->height;
    destination->scaling = source->scaling;
    destination->shadow_smoothing = source->shadow_smoothing;

    terrainHeightmapCopy(source->height_map, destination->height_map);

    destination->water_height = source->water_height;

    source->_height_noise->copy(destination->_height_noise);

    _validateDefinition(destination);
}

static void _saveDefinition(PackStream* stream, TerrainDefinition* definition)
{
    stream->write(&definition->height);
    stream->write(&definition->scaling);
    stream->write(&definition->shadow_smoothing);
    terrainHeightmapSave(stream, definition->height_map);
    stream->write(&definition->water_height);
    definition->_height_noise->save(stream);
}

static void _loadDefinition(PackStream* stream, TerrainDefinition* definition)
{
    stream->read(&definition->height);
    stream->read(&definition->scaling);
    stream->read(&definition->shadow_smoothing);
    terrainHeightmapLoad(stream, definition->height_map);
    stream->read(&definition->water_height);
    definition->_height_noise->load(stream);

    _validateDefinition(definition);
}

StandardDefinition TerrainDefinitionClass = {
    (FuncObjectCreate)_createDefinition,
    (FuncObjectDelete)_deleteDefinition,
    (FuncObjectCopy)_copyDefinition,
    (FuncObjectValidate)_validateDefinition,
    (FuncObjectSave)_saveDefinition,
    (FuncObjectLoad)_loadDefinition
};

/******************** Public tools ********************/
double terrainGetGridHeight(TerrainDefinition* definition, int x, int z, int with_painting)
{
    double height;

    if (!with_painting || !terrainHeightmapGetGridHeight(definition->height_map, x, z, &height))
    {
        height = definition->_height_noise->get2DTotal((double)x, (double)z);
    }

    return height;
}

double terrainGetInterpolatedHeight(TerrainDefinition* definition, double x, double z, int scaled, int with_painting)
{
    double height;
    x /= definition->scaling;
    z /= definition->scaling;

    if (!with_painting || !terrainHeightmapGetInterpolatedHeight(definition->height_map, x, z, &height))
    {
        height = definition->_height_noise->get2DTotal(x, z);
    }

    if (scaled)
    {
        return height * definition->height * definition->scaling;
    }
    else
    {
        return height;
    }
}

HeightInfo terrainGetHeightInfo(TerrainDefinition* definition)
{
    HeightInfo result;

    result.min_height = definition->_min_height;
    result.max_height = definition->_max_height;
    /* TODO This is duplicated in ter_render.c (_realGetWaterHeight) */
    result.base_height = definition->water_height * definition->height * definition->scaling;

    return result;
}
