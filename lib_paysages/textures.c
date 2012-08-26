#include "textures.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "shared/types.h"
#include "color.h"
#include "euclid.h"
#include "lighting.h"
#include "terrain.h"
#include "tools.h"

#define TEXTURES_MAX_LAYERS 50

typedef struct
{
    double thickness;
    Vector3 location;
    Vector3 normal;
    TextureLayerDefinition* definition;
} TextureResult;

static void texturesLayerSave(PackStream* stream, TextureLayerDefinition* layer)
{
    zoneSave(stream, layer->zone);
    noiseSaveGenerator(stream, layer->bump_noise);
    packWriteDouble(stream, &layer->bump_height);
    packWriteDouble(stream, &layer->bump_scaling);
    materialSave(stream, &layer->material);
    packWriteDouble(stream, &layer->thickness);
    packWriteDouble(stream, &layer->slope_range);
    packWriteDouble(stream, &layer->thickness_transparency);
}

static void texturesLayerLoad(PackStream* stream, TextureLayerDefinition* layer)
{
    zoneLoad(stream, layer->zone);
    noiseLoadGenerator(stream, layer->bump_noise);
    packReadDouble(stream, &layer->bump_height);
    packReadDouble(stream, &layer->bump_scaling);
    materialLoad(stream, &layer->material);
    packReadDouble(stream, &layer->thickness);
    packReadDouble(stream, &layer->slope_range);
    packReadDouble(stream, &layer->thickness_transparency);
}

TexturesDefinition texturesCreateDefinition()
{
    TexturesDefinition result;

    result.layers = layersCreate(texturesGetLayerType(), TEXTURES_MAX_LAYERS);

    return result;
}

void texturesDeleteDefinition(TexturesDefinition* definition)
{
    layersDelete(definition->layers);
}

void texturesCopyDefinition(TexturesDefinition* source, TexturesDefinition* destination)
{
    layersCopy(source->layers, destination->layers);
}

void texturesValidateDefinition(TexturesDefinition* definition)
{
    layersValidate(definition->layers);
}

void texturesSave(PackStream* stream, TexturesDefinition* definition)
{
    layersSave(stream, definition->layers);
}

void texturesLoad(PackStream* stream, TexturesDefinition* definition)
{
    layersLoad(stream, definition->layers);
}

LayerType texturesGetLayerType()
{
    LayerType result;
    
    result.callback_create = (LayerCallbackCreate)texturesLayerCreateDefinition;
    result.callback_delete = (LayerCallbackDelete)texturesLayerDeleteDefinition;
    result.callback_copy = (LayerCallbackCopy)texturesLayerCopyDefinition;
    result.callback_validate = (LayerCallbackValidate)texturesLayerValidateDefinition;
    result.callback_save = (LayerCallbackSave)texturesLayerSave;
    result.callback_load = (LayerCallbackLoad)texturesLayerLoad;
    
    return result;
}

TextureLayerDefinition* texturesLayerCreateDefinition()
{
    TextureLayerDefinition* result;

    result = malloc(sizeof(TextureLayerDefinition));
    
    result->zone = zoneCreate();
    result->bump_noise = noiseCreateGenerator();
    noiseGenerateBaseNoise(result->bump_noise, 102400);
    noiseAddLevelsSimple(result->bump_noise, 8, 1.0, 1.0);
    result->bump_height = 0.1;
    result->bump_scaling = 0.1;
    result->material.base = COLOR_WHITE;
    result->material.reflection = 0.0;
    result->material.shininess = 0.0;
    result->thickness = 0.0;
    result->slope_range = 0.001;
    result->thickness_transparency = 0.0;

    return result;
}

void texturesLayerDeleteDefinition(TextureLayerDefinition* definition)
{
    zoneDelete(definition->zone);
    noiseDeleteGenerator(definition->bump_noise);
    free(definition);
}

void texturesLayerCopyDefinition(TextureLayerDefinition* source, TextureLayerDefinition* destination)
{
    destination->material = source->material;
    destination->bump_height = source->bump_height;
    destination->bump_scaling = source->bump_scaling;
    destination->thickness = source->thickness;
    destination->slope_range = source->slope_range;
    destination->thickness_transparency = source->thickness_transparency;
    noiseCopy(source->bump_noise, destination->bump_noise);
    zoneCopy(source->zone, destination->zone);
}

void texturesLayerValidateDefinition(TextureLayerDefinition* definition)
{
    if (definition->bump_scaling < 0.000001)
    {
        definition->bump_scaling = 0.000001;
    }
    if (definition->slope_range < 0.001)
    {
        definition->slope_range = 0.001;
    }
}

static inline Vector3 _getNormal4(Vector3 center, Vector3 north, Vector3 east, Vector3 south, Vector3 west)
{
    Vector3 dnorth, deast, dsouth, dwest, normal;
    
    dnorth = v3Sub(north, center);
    deast = v3Sub(east, center);
    dsouth = v3Sub(south, center);
    dwest = v3Sub(west, center);
    
    normal = v3Cross(deast, dnorth);
    normal = v3Add(normal, v3Cross(dsouth, deast));
    normal = v3Add(normal, v3Cross(dwest, dsouth));
    normal = v3Add(normal, v3Cross(dnorth, dwest));
    
    return v3Normalize(normal);
}

static inline Vector3 _getNormal2(Vector3 center, Vector3 east, Vector3 south)
{
    return v3Normalize(v3Cross(v3Sub(south, center), v3Sub(east, center)));
}

static inline TextureResult _getTerrainResult(Renderer* renderer, double x, double z, double detail)
{
    TextureResult result;
    Vector3 center, north, east, south, west;
    
    /* TODO This method is better suited in terrain.c */

    center.x = x;
    center.z = z;
    center.y = renderer->getTerrainHeight(renderer, center.x, center.z);

    east.x = x + detail;
    east.z = z;
    east.y = renderer->getTerrainHeight(renderer, east.x, east.z);

    south.x = x;
    south.z = z + detail;
    south.y = renderer->getTerrainHeight(renderer, south.x, south.z);

    if (renderer->render_quality > 5)
    {
        west.x = x - detail;
        west.z = z;
        west.y = renderer->getTerrainHeight(renderer, west.x, west.z);

        north.x = x;
        north.z = z - detail;
        north.y = renderer->getTerrainHeight(renderer, north.x, north.z);

        result.normal = _getNormal4(center, north, east, south, west);
    }
    else
    {
        result.normal = _getNormal2(center, east, south);
    }

    result.location = center;
    result.thickness = -100.0;
    result.definition = NULL;
    
    return result;
}

static inline void _getLayerThickness(TextureLayerDefinition* definition, Renderer* renderer, double x, double z, TextureResult* result)
{
    TextureResult base;
    double coverage;

    base = _getTerrainResult(renderer, x, z, definition->slope_range);
    coverage = zoneGetValue(definition->zone, base.location, base.normal);
    if (coverage > 0.0)
    {
        result->thickness = coverage * definition->thickness;
        result->thickness += noiseGet2DTotal(definition->bump_noise, base.location.x / definition->bump_scaling, base.location.z / definition->bump_scaling) * definition->bump_height;

        result->location = v3Add(base.location, v3Scale(base.normal, result->thickness));
    }
    else
    {
        result->thickness = -1000.0;
        result->location = base.location;
    }
}

static inline TextureResult _getLayerResult(TextureLayerDefinition* definition, Renderer* renderer, double x, double z, double detail)
{
    TextureResult result_center, result_north, result_east, result_south, result_west;
    
    _getLayerThickness(definition, renderer, x, z, &result_center);
    _getLayerThickness(definition, renderer, x + detail, z, &result_east);
    _getLayerThickness(definition, renderer, x, z + detail, &result_south);
    
    if (renderer->render_quality > 5)
    {
        _getLayerThickness(definition, renderer, x - detail, z, &result_west);
        _getLayerThickness(definition, renderer, x, z - detail, &result_north);
        
        result_center.normal = _getNormal4(result_center.location, result_north.location, result_east.location, result_south.location, result_west.location);
    }
    else
    {
        result_center.normal = _getNormal2(result_center.location, result_east.location, result_south.location);
    }
    
    result_center.definition = definition;
    
    return result_center;
}

static int _cmpResults(const void* result1, const void* result2)
{
    return ((TextureResult*)result1)->thickness > ((TextureResult*)result2)->thickness;
}

double texturesGetLayerCoverage(TextureLayerDefinition* definition, Renderer* renderer, Vector3 location, double detail)
{
    TextureResult base = _getTerrainResult(renderer, location.x, location.z, definition->slope_range);
    return zoneGetValue(definition->zone, base.location, base.normal);
}

static inline Color _getLayerColor(Renderer* renderer, TextureResult result, LightStatus* light)
{
    return renderer->applyLightStatus(renderer, light, result.location, result.normal, result.definition->material);
}

Color texturesGetLayerColor(TextureLayerDefinition* definition, Renderer* renderer, Vector3 location, double detail)
{
    LightStatus light;
    TextureResult result = _getLayerResult(definition, renderer, location.x, location.z, detail);
    renderer->getLightStatus(renderer, &light, result.location);
    return _getLayerColor(renderer, result, &light);
}

Color texturesGetColor(TexturesDefinition* definition, Renderer* renderer, double x, double z, double detail)
{
    TextureResult results[TEXTURES_MAX_LAYERS + 1];
    Color result, color;
    double thickness, last_height;
    int i, start, nblayers;

    detail *= 0.1;
    
    results[0] = _getTerrainResult(renderer, x, z, detail);

    nblayers = layersCount(definition->layers);
    for (i = 0; i < nblayers; i++)
    {
        results[i + 1] = _getLayerResult(layersGetLayer(definition->layers, i), renderer, x, z, detail);
    }
    
    qsort(results, nblayers + 1, sizeof(TextureResult), _cmpResults);

    /* Pre compute alpha channel */
    start = 0;
    last_height = results[0].thickness;
    results[0].thickness = 1.0;
    for (i = 1; i <= nblayers; i++)
    {
        thickness = results[i].thickness - last_height;
        last_height = results[i].thickness;
        
        if (results[i].definition)
        {
            if (thickness < results[i].definition->thickness_transparency)
            {
                results[i].thickness = thickness / results[i].definition->thickness_transparency;
            }
            else
            {
                results[i].thickness = (thickness > 0.0) ? 1.0 : 0.0;
            }
        }
        else
        {
            results[i].thickness = 1.0;
        }
        
        if (results[i].thickness >= 0.999999)
        {
            start = i;
        }
    }

    /* Apply colors and alphas */
    LightStatus light;
    renderer->getLightStatus(renderer, &light, results[start].location);
    if (results[start].definition)
    {
        result = _getLayerColor(renderer, results[start], &light);
    }
    else
    {
        result = COLOR_GREEN;
    }
    for (i = start + 1; i <= nblayers; i++)
    {
        if (results[i].thickness)
        {
            if (results[i].definition)
            {
                color = _getLayerColor(renderer, results[i], &light);
                color.a = results[i].thickness;
            }
            else
            {
                color = COLOR_GREEN;
            }

            colorMask(&result, &color);
        }
    }
    
    return result;
}
