#include "private.h"

#include <stdlib.h>
#include <math.h>
#include "../tools.h"
#include "../renderer.h"

/******************** Definition ********************/
static void _validateDefinition(TerrainDefinition* definition)
{
    noiseValidate(definition->_height_noise);

    if (definition->height < 1.0)
    {
        definition->height = 1.0;
    }

    /* Get minimal and maximal height */
    noiseGetRange(definition->_height_noise, &definition->_min_height, &definition->_max_height);
    definition->_min_height *= definition->height * definition->scaling;
    definition->_max_height *= definition->height * definition->scaling;

    /* TODO Alter with heightmap min/max */
}

static TerrainDefinition* _createDefinition()
{
    TerrainDefinition* definition = malloc(sizeof(TerrainDefinition));

    definition->height = 1.0;
    definition->scaling = 1.0;
    definition->shadow_smoothing = 0.0;

    definition->height_map = terrainHeightMapCreate(definition);

    definition->water_height = -0.3;

    definition->_height_noise = noiseCreateGenerator();

    terrainAutoPreset(definition, TERRAIN_PRESET_STANDARD);

    return definition;
}

static void _deleteDefinition(TerrainDefinition* definition)
{
    terrainHeightmapDelete(definition->height_map);
    noiseDeleteGenerator(definition->_height_noise);
    free(definition);
}

static void _copyDefinition(TerrainDefinition* source, TerrainDefinition* destination)
{
    destination->height = source->height;
    destination->scaling = source->scaling;
    destination->shadow_smoothing = source->shadow_smoothing;

    terrainHeightmapCopy(source->height_map, destination->height_map);

    destination->water_height = source->water_height;

    noiseCopy(source->_height_noise, destination->_height_noise);

    _validateDefinition(destination);
}

static void _saveDefinition(PackStream* stream, TerrainDefinition* definition)
{
    packWriteDouble(stream, &definition->height);
    packWriteDouble(stream, &definition->scaling);
    packWriteDouble(stream, &definition->shadow_smoothing);
    terrainHeightmapSave(stream, definition->height_map);
    packWriteDouble(stream, &definition->water_height);
    noiseSaveGenerator(stream, definition->_height_noise);
}

static void _loadDefinition(PackStream* stream, TerrainDefinition* definition)
{
    packReadDouble(stream, &definition->height);
    packReadDouble(stream, &definition->scaling);
    packReadDouble(stream, &definition->shadow_smoothing);
    terrainHeightmapLoad(stream, definition->height_map);
    packReadDouble(stream, &definition->water_height);
    noiseLoadGenerator(stream, definition->_height_noise);

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
        height = noiseGet2DTotal(definition->_height_noise, (double)x, (double)z);
    }

    return height * definition->height * definition->scaling;
}

double terrainGetInterpolatedHeight(TerrainDefinition* definition, double x, double z, int with_painting)
{
    double height;
    x /= definition->scaling;
    z /= definition->scaling;

    if (!with_painting || !terrainHeightmapGetInterpolatedHeight(definition->height_map, x, z, &height))
    {
        height = noiseGet2DTotal(definition->_height_noise, x, z);
    }

    return height * definition->height * definition->scaling;
}
