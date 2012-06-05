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

static TextureLayerDefinition _NULL_LAYER;

typedef struct
{
    double thickness;
    Vector3 location;
    Vector3 normal;
    TextureLayerDefinition* definition;
} TextureResult;

void texturesInit()
{
    _NULL_LAYER = texturesLayerCreateDefinition();
}

void texturesQuit()
{
    texturesLayerDeleteDefinition(&_NULL_LAYER);
}

void texturesSave(PackStream* stream, TexturesDefinition* definition)
{
    int i;

    packWriteInt(stream, &definition->nbtextures);
    for (i = 0; i < definition->nbtextures; i++)
    {
        zoneSave(stream, definition->textures[i].zone);
        noiseSaveGenerator(stream, definition->textures[i].bump_noise);
        packWriteDouble(stream, &definition->textures[i].bump_height);
        packWriteDouble(stream, &definition->textures[i].bump_scaling);
        materialSave(stream, &definition->textures[i].material);
        packWriteDouble(stream, &definition->textures[i].thickness);
        packWriteDouble(stream, &definition->textures[i].slope_range);
        packWriteDouble(stream, &definition->textures[i].thickness_transparency);
    }
}

void texturesLoad(PackStream* stream, TexturesDefinition* definition)
{
    TextureLayerDefinition* layer;
    int i, n;

    while (definition->nbtextures > 0)
    {
        texturesDeleteLayer(definition, 0);
    }

    packReadInt(stream, &n);
    for (i = 0; i < n; i++)
    {
        layer = definition->textures + texturesAddLayer(definition);

        zoneLoad(stream, layer->zone);
        noiseLoadGenerator(stream, layer->bump_noise);
        packReadDouble(stream, &layer->bump_height);
        packReadDouble(stream, &layer->bump_scaling);
        materialLoad(stream, &layer->material);
        packReadDouble(stream, &definition->textures[i].thickness);
        packReadDouble(stream, &definition->textures[i].slope_range);
        packReadDouble(stream, &definition->textures[i].thickness_transparency);
    }

    texturesValidateDefinition(definition);
}

TexturesDefinition texturesCreateDefinition()
{
    TexturesDefinition result;

    result.nbtextures = 0;

    return result;
}

void texturesDeleteDefinition(TexturesDefinition* definition)
{
    while (definition->nbtextures > 0)
    {
        texturesDeleteLayer(definition, 0);
    }
}

void texturesCopyDefinition(TexturesDefinition* source, TexturesDefinition* destination)
{
    TextureLayerDefinition* layer;
    int i;

    while (destination->nbtextures > 0)
    {
        texturesDeleteLayer(destination, 0);
    }
    for (i = 0; i < source->nbtextures; i++)
    {
        layer = texturesGetLayer(destination, texturesAddLayer(destination));
        texturesLayerCopyDefinition(source->textures + i, layer);
    }
}

void texturesValidateDefinition(TexturesDefinition* definition)
{
    int i;
    for (i = 0; i < definition->nbtextures; i++)
    {
        texturesLayerValidateDefinition(definition->textures + i);
    }
}

TextureLayerDefinition texturesLayerCreateDefinition()
{
    TextureLayerDefinition result;

    result.zone = zoneCreate();
    result.bump_noise = noiseCreateGenerator();
    noiseGenerateBaseNoise(result.bump_noise, 102400);
    noiseAddLevelsSimple(result.bump_noise, 8, 1.0, 1.0);
    result.bump_height = 0.1;
    result.bump_scaling = 0.1;
    result.material.base = COLOR_WHITE;
    result.material.reflection = 0.0;
    result.material.shininess = 0.0;
    result.thickness = 0.0;
    result.slope_range = 0.001;
    result.thickness_transparency = 0.0;

    return result;
}

void texturesLayerDeleteDefinition(TextureLayerDefinition* definition)
{
    zoneDelete(definition->zone);
    noiseDeleteGenerator(definition->bump_noise);
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

int texturesGetLayerCount(TexturesDefinition* definition)
{
    return definition->nbtextures;
}

TextureLayerDefinition* texturesGetLayer(TexturesDefinition* definition, int layer)
{
    if (layer >= 0 && layer < definition->nbtextures)
    {
        return definition->textures + layer;
    }
    else
    {
        return &_NULL_LAYER;
    }
}

int texturesAddLayer(TexturesDefinition* definition)
{
    if (definition->nbtextures < TEXTURES_MAX_LAYERS)
    {
        definition->textures[definition->nbtextures] = texturesLayerCreateDefinition();

        return definition->nbtextures++;
    }
    else
    {
        return -1;
    }
}

void texturesDeleteLayer(TexturesDefinition* definition, int layer)
{
    if (layer >= 0 && layer < definition->nbtextures)
    {
        texturesLayerDeleteDefinition(definition->textures + layer);
        if (definition->nbtextures > 1 && layer < definition->nbtextures - 1)
        {
            memmove(definition->textures + layer, definition->textures + layer + 1, sizeof(TextureLayerDefinition) * (definition->nbtextures - layer - 1));
        }
        definition->nbtextures--;
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

static inline Color _getLayerColor(Renderer* renderer, TextureResult result)
{
    LightStatus light;
    
    renderer->getLightStatus(renderer, &light, result.location);
    return renderer->applyLightStatus(renderer, &light, result.location, result.normal, result.definition->material);
}

Color texturesGetLayerColor(TextureLayerDefinition* definition, Renderer* renderer, Vector3 location, double detail)
{
    return _getLayerColor(renderer, _getLayerResult(definition, renderer, location.x, location.z, detail));
}

Color texturesGetColor(TexturesDefinition* definition, Renderer* renderer, double x, double z, double detail)
{
    TextureResult results[TEXTURES_MAX_LAYERS + 1];
    Color result, color;
    double thickness, last_height;
    int i, start;

    /* TODO Optimize : each layer computes the same shadows */
    
    detail *= 0.1;
    
    results[0] = _getTerrainResult(renderer, x, z, detail);

    for (i = 0; i < definition->nbtextures; i++)
    {
        results[i + 1] = _getLayerResult(definition->textures + i, renderer, x, z, detail);
    }
    
    qsort(results, definition->nbtextures + 1, sizeof(TextureResult), _cmpResults);

    /* Pre compute alpha channel */
    start = 0;
    last_height = results[0].thickness;
    for (i = 1; i <= definition->nbtextures; i++)
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
            color = COLOR_GREEN;
            results[i].thickness = 1.0;
        }
        
        if (results[i].thickness >= 0.999999)
        {
            start = i;
        }
        
        colorMask(&result, &color);
    }

    /* Apply colors and alphas */
    if (results[start].definition)
    {
        result = _getLayerColor(renderer, results[start]);
    }
    else
    {
        result = COLOR_GREEN;
    }
    for (i = start; i <= definition->nbtextures; i++)
    {
        if (results[i].thickness)
        {
            if (results[i].definition)
            {
                color = _getLayerColor(renderer, results[i]);
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
