#include "private.h"
#include "Scenery.h"

#include <stdlib.h>

/******************** Global definition ********************/
static void _validateDefinition(TexturesDefinition* definition)
{
    layersValidate(definition->layers);
}

static TexturesDefinition* _createDefinition()
{
    TexturesDefinition* definition = malloc(sizeof(TexturesDefinition));

    definition->layers = layersCreate(texturesGetLayerType(), TEXTURES_MAX_LAYERS);

    return definition;
}

static void _deleteDefinition(TexturesDefinition* definition)
{
    layersDelete(definition->layers);
    free(definition);
}

static void _copyDefinition(TexturesDefinition* source, TexturesDefinition* destination)
{
    layersCopy(source->layers, destination->layers);
}

static void _saveDefinition(PackStream* stream, TexturesDefinition* definition)
{
    layersSave(stream, definition->layers);
}

static void _loadDefinition(PackStream* stream, TexturesDefinition* definition)
{
    layersLoad(stream, definition->layers);
}

StandardDefinition TexturesDefinitionClass = {
    (FuncObjectCreate)_createDefinition,
    (FuncObjectDelete)_deleteDefinition,
    (FuncObjectCopy)_copyDefinition,
    (FuncObjectValidate)_validateDefinition,
    (FuncObjectSave)_saveDefinition,
    (FuncObjectLoad)_loadDefinition
};

/*** Layer definition ***/

static void _layerValidateDefinition(TexturesLayerDefinition* definition)
{
    if (definition->displacement_scaling < 0.000001)
    {
        definition->displacement_scaling = 0.000001;
    }

    noiseClearLevels(definition->_displacement_noise);
    noiseAddLevelsSimple(definition->_displacement_noise, 9, 1.0, -1.0, 1.0, 0.0);
    noiseNormalizeAmplitude(definition->_displacement_noise, -1.0, 1.0, 0);
    noiseValidate(definition->_displacement_noise);

    noiseClearLevels(definition->_detail_noise);
    noiseAddLevelsSimple(definition->_detail_noise, 7, 0.01, -1.0, 1.0, 0.0);
    noiseNormalizeAmplitude(definition->_detail_noise, -0.008, 0.008, 0);
    noiseValidate(definition->_detail_noise);

    materialValidate(&definition->material);

    /* Update zone height range */
    TerrainDefinition* terrain = sceneryGetTerrainDirect();
    HeightInfo height_info = terrainGetHeightInfo(terrain);
    zoneSetRelativeHeight(definition->terrain_zone, height_info.min_height, height_info.base_height, height_info.max_height);
}

static TexturesLayerDefinition* _layerCreateDefinition()
{
    TexturesLayerDefinition* result;

    result = malloc(sizeof(TexturesLayerDefinition));

    result->terrain_zone = zoneCreate();
    result->_displacement_noise = noiseCreateGenerator();
    result->_detail_noise = noiseCreateGenerator();

    texturesLayerAutoPreset(result, TEXTURES_LAYER_PRESET_ROCK);

    _layerValidateDefinition(result);

    return result;
}

static void _layerDeleteDefinition(TexturesLayerDefinition* definition)
{
    zoneDelete(definition->terrain_zone);
    noiseDeleteGenerator(definition->_displacement_noise);
    noiseDeleteGenerator(definition->_detail_noise);
    free(definition);
}

static void _layerCopyDefinition(TexturesLayerDefinition* source, TexturesLayerDefinition* destination)
{
    zoneCopy(source->terrain_zone, destination->terrain_zone);
    destination->displacement_scaling = source->displacement_scaling;
    destination->displacement_height = source->displacement_height;
    destination->displacement_offset = source->displacement_offset;
    destination->material = source->material;

    noiseCopy(source->_displacement_noise, destination->_displacement_noise);
    noiseCopy(source->_detail_noise, destination->_detail_noise);
}

static void _layerSave(PackStream* stream, TexturesLayerDefinition* layer)
{
    zoneSave(stream, layer->terrain_zone);
    packWriteDouble(stream, &layer->displacement_scaling);
    packWriteDouble(stream, &layer->displacement_height);
    packWriteDouble(stream, &layer->displacement_offset);
    materialSave(stream, &layer->material);

    noiseSaveGenerator(stream, layer->_displacement_noise);
    noiseSaveGenerator(stream, layer->_detail_noise);
}

static void _layerLoad(PackStream* stream, TexturesLayerDefinition* layer)
{
    zoneLoad(stream, layer->terrain_zone);
    packReadDouble(stream, &layer->displacement_scaling);
    packReadDouble(stream, &layer->displacement_height);
    packReadDouble(stream, &layer->displacement_offset);
    materialLoad(stream, &layer->material);

    noiseLoadGenerator(stream, layer->_displacement_noise);
    noiseLoadGenerator(stream, layer->_detail_noise);
}

LayerType texturesGetLayerType()
{
    LayerType result;

    result.callback_create = (LayerCallbackCreate)_layerCreateDefinition;
    result.callback_delete = (LayerCallbackDelete)_layerDeleteDefinition;
    result.callback_copy = (LayerCallbackCopy)_layerCopyDefinition;
    result.callback_validate = (LayerCallbackValidate)_layerValidateDefinition;
    result.callback_save = (LayerCallbackSave)_layerSave;
    result.callback_load = (LayerCallbackLoad)_layerLoad;

    return result;
}
