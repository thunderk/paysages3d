#include "private.h"

#include "Scenery.h"
#include "NoiseGenerator.h"
#include "PackStream.h"

/******************** Global definition ********************/
static void _validateDefinition(TexturesDefinition* definition)
{
    layersValidate(definition->layers);
}

static TexturesDefinition* _createDefinition()
{
    TexturesDefinition* definition = new TexturesDefinition;

    definition->layers = layersCreate(texturesGetLayerType(), TEXTURES_MAX_LAYERS);

    return definition;
}

static void _deleteDefinition(TexturesDefinition* definition)
{
    layersDelete(definition->layers);
    delete definition;
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

    definition->_displacement_noise->clearLevels();
    definition->_displacement_noise->addLevelsSimple(9, 1.0, -1.0, 1.0, 0.0);
    definition->_displacement_noise->normalizeAmplitude(-1.0, 1.0, 0);
    definition->_displacement_noise->validate();

    definition->_detail_noise->clearLevels();
    definition->_detail_noise->addLevelsSimple(7, 0.01, -1.0, 1.0, 0.0);
    definition->_detail_noise->normalizeAmplitude(-0.008, 0.008, 0);
    definition->_detail_noise->validate();

    materialValidate(&definition->material);

    /* Update zone height range */
    TerrainDefinition* terrain = Scenery::getCurrent()->getTerrain();
    if (terrain)
    {
        HeightInfo height_info = terrainGetHeightInfo(terrain);
        zoneSetRelativeHeight(definition->terrain_zone, height_info.min_height, height_info.base_height, height_info.max_height);
    }
}

static TexturesLayerDefinition* _layerCreateDefinition()
{
    TexturesLayerDefinition* result;

    result = new TexturesLayerDefinition;

    result->terrain_zone = zoneCreate();
    result->_displacement_noise = new NoiseGenerator();
    result->_detail_noise = new NoiseGenerator();

    texturesLayerAutoPreset(result, TEXTURES_LAYER_PRESET_ROCK);

    _layerValidateDefinition(result);

    return result;
}

static void _layerDeleteDefinition(TexturesLayerDefinition* definition)
{
    zoneDelete(definition->terrain_zone);
    delete definition->_displacement_noise;
    delete definition->_detail_noise;
    delete definition;
}

static void _layerCopyDefinition(TexturesLayerDefinition* source, TexturesLayerDefinition* destination)
{
    zoneCopy(source->terrain_zone, destination->terrain_zone);
    destination->displacement_scaling = source->displacement_scaling;
    destination->displacement_height = source->displacement_height;
    destination->displacement_offset = source->displacement_offset;
    destination->material = source->material;

    source->_displacement_noise->copy(destination->_displacement_noise);
    source->_detail_noise->copy(destination->_detail_noise);
}

static void _layerSave(PackStream* stream, TexturesLayerDefinition* layer)
{
    zoneSave(stream, layer->terrain_zone);
    stream->write(&layer->displacement_scaling);
    stream->write(&layer->displacement_height);
    stream->write(&layer->displacement_offset);
    materialSave(stream, &layer->material);

    layer->_displacement_noise->save(stream);
    layer->_detail_noise->save(stream);
}

static void _layerLoad(PackStream* stream, TexturesLayerDefinition* layer)
{
    zoneLoad(stream, layer->terrain_zone);
    stream->read(&layer->displacement_scaling);
    stream->read(&layer->displacement_height);
    stream->read(&layer->displacement_offset);
    materialLoad(stream, &layer->material);

    layer->_displacement_noise->load(stream);
    layer->_detail_noise->load(stream);
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
