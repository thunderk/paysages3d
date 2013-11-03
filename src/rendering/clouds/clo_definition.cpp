#include "private.h"

#include <stdlib.h>
#include "PackStream.h"
#include "NoiseGenerator.h"

/******************** Global definition ********************/
static void _validateDefinition(CloudsDefinition* definition)
{
    layersValidate(definition->layers);
}

static CloudsDefinition* _createDefinition()
{
    CloudsDefinition* definition = new CloudsDefinition;

    definition->layers = layersCreate(cloudsGetLayerType(), CLOUDS_MAX_LAYERS);

    return definition;
}

static void _deleteDefinition(CloudsDefinition* definition)
{
    layersDelete(definition->layers);
    delete definition;
}

static void _copyDefinition(CloudsDefinition* source, CloudsDefinition* destination)
{
    layersCopy(source->layers, destination->layers);
}

static void _saveDefinition(PackStream* stream, CloudsDefinition* definition)
{
    layersSave(stream, definition->layers);
}

static void _loadDefinition(PackStream* stream, CloudsDefinition* definition)
{
    layersLoad(stream, definition->layers);
}

StandardDefinition CloudsDefinitionClass = {
    (FuncObjectCreate)_createDefinition,
    (FuncObjectDelete)_deleteDefinition,
    (FuncObjectCopy)_copyDefinition,
    (FuncObjectValidate)_validateDefinition,
    (FuncObjectSave)_saveDefinition,
    (FuncObjectLoad)_loadDefinition
};

/*** Layer definition ***/

void cloudsLayerValidateDefinition(CloudsLayerDefinition* definition)
{
    if (definition->shape_scaling < 0.0001)
    {
        definition->shape_scaling = 0.00001;
    }
    if (definition->edge_scaling < 0.0001)
    {
        definition->edge_scaling = 0.00001;
    }

    curveClear(definition->_coverage_by_altitude);
    definition->_shape_noise->clearLevels();
    definition->_edge_noise->clearLevels();
    definition->_coverage_noise->clearLevels();

    definition->_coverage_noise->addLevelsSimple(2, 10.0, 0.0, 1.0, 0.0);
    definition->_coverage_noise->addLevelsSimple(2, 1.0, 0.0, 1.0, 0.0);
    definition->_coverage_noise->setFunctionParams(NOISE_FUNCTION_NAIVE, 0.0, 0.0);
    switch (definition->type)
    {
        case CLOUDS_TYPE_CIRRUS:
            curveQuickAddPoint(definition->_coverage_by_altitude, 0.0, 0.0);
            curveQuickAddPoint(definition->_coverage_by_altitude, 0.5, 1.0);
            curveQuickAddPoint(definition->_coverage_by_altitude, 1.0, 0.0);
            definition->_shape_noise->addLevelsSimple(3, 1.0, 0.0, 1.0, 0.5);
            definition->_shape_noise->setFunctionParams(NOISE_FUNCTION_SIMPLEX, 0.0, 0.0);
            definition->_edge_noise->addLevelsSimple(4, 1.0, -0.5, 0.5, 0.5);
            definition->_edge_noise->setFunctionParams(NOISE_FUNCTION_SIMPLEX, -0.2, 0.0);
            break;
        case CLOUDS_TYPE_CUMULUS:
            curveQuickAddPoint(definition->_coverage_by_altitude, 0.0, 0.0);
            curveQuickAddPoint(definition->_coverage_by_altitude, 0.1, 1.0);
            curveQuickAddPoint(definition->_coverage_by_altitude, 0.4, 0.8);
            curveQuickAddPoint(definition->_coverage_by_altitude, 0.7, 1.0);
            curveQuickAddPoint(definition->_coverage_by_altitude, 1.0, 0.0);
            definition->_shape_noise->addLevelsSimple(7, 1.0, 0.0, 1.0, 0.5);
            definition->_shape_noise->setFunctionParams(NOISE_FUNCTION_SIMPLEX, 0.4, 0.0);
            definition->_edge_noise->addLevelsSimple(4, 1.0, -0.5, 0.5, 0.5);
            definition->_edge_noise->setFunctionParams(NOISE_FUNCTION_SIMPLEX, 0.8, 0.0);
            break;
        case CLOUDS_TYPE_STRATOCUMULUS:
            curveQuickAddPoint(definition->_coverage_by_altitude, 0.0, 0.0);
            curveQuickAddPoint(definition->_coverage_by_altitude, 0.2, 1.0);
            curveQuickAddPoint(definition->_coverage_by_altitude, 0.5, 1.0);
            curveQuickAddPoint(definition->_coverage_by_altitude, 1.0, 0.0);
            definition->_shape_noise->addLevelsSimple(4, 1.0, 0.0, 1.0, 0.5);
            definition->_shape_noise->setFunctionParams(NOISE_FUNCTION_SIMPLEX, 0.3, 0.0);
            definition->_edge_noise->addLevelsSimple(6, 1.0, -0.5, 0.5, 0.5);
            definition->_edge_noise->setFunctionParams(NOISE_FUNCTION_SIMPLEX, 0.5, 0.0);
            break;
        case CLOUDS_TYPE_STRATUS:
            curveQuickAddPoint(definition->_coverage_by_altitude, 0.0, 0.0);
            curveQuickAddPoint(definition->_coverage_by_altitude, 0.2, 1.0);
            curveQuickAddPoint(definition->_coverage_by_altitude, 0.8, 1.0);
            curveQuickAddPoint(definition->_coverage_by_altitude, 1.0, 0.0);
            definition->_shape_noise->addLevelsSimple(3, 1.0, 0.0, 1.0, 0.5);
            definition->_shape_noise->setFunctionParams(NOISE_FUNCTION_SIMPLEX, -0.3, 0.0);
            definition->_edge_noise->addLevelsSimple(4, 1.0, -0.5, 0.5, 0.5);
            definition->_edge_noise->setFunctionParams(NOISE_FUNCTION_SIMPLEX, -0.5, 0.0);
            break;
        default:
            break;
    }

    definition->_coverage_noise->normalizeAmplitude(-1.0, 3.0, 0);
    definition->_shape_noise->normalizeAmplitude(-0.5, 0.5, 0);
    definition->_edge_noise->normalizeAmplitude(-0.5, 0.5, 0);

    materialValidate(&definition->material);
}

CloudsLayerDefinition* cloudsLayerCreateDefinition()
{
    CloudsLayerDefinition* result;

    result = new CloudsLayerDefinition;
    result->_coverage_by_altitude = curveCreate();
    result->_coverage_noise = new NoiseGenerator();
    result->_shape_noise = new NoiseGenerator();
    result->_edge_noise = new NoiseGenerator();

    cloudsLayerAutoPreset(result, CLOUDS_LAYER_PRESET_CIRRUS);

    return result;
}

void cloudsLayerDeleteDefinition(CloudsLayerDefinition* definition)
{
    curveDelete(definition->_coverage_by_altitude);
    delete definition->_coverage_noise;
    delete definition->_shape_noise;
    delete definition->_edge_noise;
    delete definition;
}

void cloudsLayerCopyDefinition(CloudsLayerDefinition* source, CloudsLayerDefinition* destination)
{
    CloudsLayerDefinition temp;

    temp = *destination;
    *destination = *source;

    destination->_coverage_by_altitude = temp._coverage_by_altitude;
    curveCopy(source->_coverage_by_altitude, destination->_coverage_by_altitude);

    destination->_coverage_noise = temp._coverage_noise;
    source->_coverage_noise->copy(destination->_coverage_noise);

    destination->_shape_noise = temp._shape_noise;
    source->_shape_noise->copy(destination->_shape_noise);

    destination->_edge_noise = temp._edge_noise;
    source->_edge_noise->copy(destination->_edge_noise);
}

void _cloudsLayerSave(PackStream* stream, CloudsLayerDefinition* layer)
{
    int clouds_type = (int)layer->type;

    stream->write(&clouds_type);
    stream->write(&layer->lower_altitude);
    stream->write(&layer->thickness);
    curveSave(stream, layer->_coverage_by_altitude);
    layer->_coverage_noise->save(stream);
    layer->_shape_noise->save(stream);
    layer->_edge_noise->save(stream);
    materialSave(stream, &layer->material);
    stream->write(&layer->hardness);
    stream->write(&layer->transparencydepth);
    stream->write(&layer->lighttraversal);
    stream->write(&layer->minimumlight);
    stream->write(&layer->shape_scaling);
    stream->write(&layer->edge_scaling);
    stream->write(&layer->edge_length);
    stream->write(&layer->base_coverage);
}

void _cloudsLayerLoad(PackStream* stream, CloudsLayerDefinition* layer)
{
    int clouds_type;

    stream->read(&clouds_type);
    layer->type = (CloudsType)clouds_type;
    stream->read(&layer->lower_altitude);
    stream->read(&layer->thickness);
    curveLoad(stream, layer->_coverage_by_altitude);
    layer->_coverage_noise->load(stream);
    layer->_shape_noise->load(stream);
    layer->_edge_noise->load(stream);
    materialLoad(stream, &layer->material);
    stream->read(&layer->hardness);
    stream->read(&layer->transparencydepth);
    stream->read(&layer->lighttraversal);
    stream->read(&layer->minimumlight);
    stream->read(&layer->shape_scaling);
    stream->read(&layer->edge_scaling);
    stream->read(&layer->edge_length);
    stream->read(&layer->base_coverage);

    cloudsLayerValidateDefinition(layer);
}

LayerType cloudsGetLayerType()
{
    LayerType result;

    result.callback_create = (LayerCallbackCreate)cloudsLayerCreateDefinition;
    result.callback_delete = (LayerCallbackDelete)cloudsLayerDeleteDefinition;
    result.callback_copy = (LayerCallbackCopy)cloudsLayerCopyDefinition;
    result.callback_validate = (LayerCallbackValidate)cloudsLayerValidateDefinition;
    result.callback_save = (LayerCallbackSave)_cloudsLayerSave;
    result.callback_load = (LayerCallbackLoad)_cloudsLayerLoad;

    return result;
}
