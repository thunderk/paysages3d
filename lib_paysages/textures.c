#include "textures.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "shared/types.h"
#include "shared/constants.h"
#include "color.h"
#include "euclid.h"
#include "lighting.h"
#include "terrain.h"
#include "tools.h"

static TextureLayerDefinition _NULL_LAYER;

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
    result.bump_height = 0.0;
    result.bump_scaling = 0.0;
    result.material.base = COLOR_GREEN;
    result.material.reflection = 0.0;
    result.material.shininess = 0.0;

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
    noiseCopy(source->bump_noise, destination->bump_noise);
    zoneCopy(source->zone, destination->zone);
}

void texturesLayerValidateDefinition(TextureLayerDefinition* definition)
{
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

static inline Vector3 _getNormal(TextureLayerDefinition* definition, Renderer* renderer, Vector3 point, double scale)
{
    Vector3 dpoint, ref, normal;

    ref.x = 0.0;
    ref.y = 0.0;
    point.y = renderer->getTerrainHeight(renderer, point.x, point.z) + noiseGet2DTotal(definition->bump_noise, point.x / definition->bump_scaling, point.z / definition->bump_scaling) * definition->bump_height;

    dpoint.x = point.x - scale;
    dpoint.z = point.z;
    dpoint.y = renderer->getTerrainHeight(renderer, dpoint.x, dpoint.z) + noiseGet2DTotal(definition->bump_noise, dpoint.x / definition->bump_scaling, dpoint.z / definition->bump_scaling) * definition->bump_height;
    ref.z = -1.0;
    normal = v3Normalize(v3Cross(ref, v3Sub(dpoint, point)));

    dpoint.x = point.x + scale;
    dpoint.z = point.z;
    dpoint.y = renderer->getTerrainHeight(renderer, dpoint.x, dpoint.z) + noiseGet2DTotal(definition->bump_noise, dpoint.x / definition->bump_scaling, dpoint.z / definition->bump_scaling) * definition->bump_height;
    ref.z = 1.0;
    normal = v3Add(normal, v3Normalize(v3Cross(ref, v3Sub(dpoint, point))));

    ref.z = 0.0;

    dpoint.x = point.x;
    dpoint.z = point.z - scale;
    dpoint.y = renderer->getTerrainHeight(renderer, dpoint.x, dpoint.z) + noiseGet2DTotal(definition->bump_noise, dpoint.x / definition->bump_scaling, dpoint.z / definition->bump_scaling) * definition->bump_height;
    ref.x = 1.0;
    normal = v3Add(normal, v3Normalize(v3Cross(ref, v3Sub(dpoint, point))));

    dpoint.x = point.x;
    dpoint.z = point.z + scale;
    dpoint.y = renderer->getTerrainHeight(renderer, dpoint.x, dpoint.z) + noiseGet2DTotal(definition->bump_noise, dpoint.x / definition->bump_scaling, dpoint.z / definition->bump_scaling) * definition->bump_height;
    ref.x = -1.0;
    normal = v3Add(normal, v3Normalize(v3Cross(ref, v3Sub(dpoint, point))));

    return v3Normalize(normal);
}

double texturesGetLayerCoverage(TextureLayerDefinition* definition, Renderer* renderer, Vector3 location, double detail)
{
    Vector3 normal;

    normal = _getNormal(definition, renderer, location, detail * 0.1);

    return zoneGetValue(definition->zone, location, normal);
}

Color texturesGetLayerColor(TextureLayerDefinition* definition, Renderer* renderer, Vector3 location, double detail)
{
    Color result;
    Vector3 normal;
    double coverage;

    result = COLOR_TRANSPARENT;
    normal = _getNormal(definition, renderer, location, detail * 0.1);

    coverage = zoneGetValue(definition->zone, location, normal);
    if (coverage > 0.0)
    {
        result = renderer->applyLightingToSurface(renderer, location, normal, definition->material);
        result.a = coverage;
    }
    return result;
}

Color texturesGetColor(TexturesDefinition* definition, Renderer* renderer, Vector3 location, double detail)
{
    Color result, tex_color;
    int i;

    result = COLOR_GREEN;
    for (i = 0; i < definition->nbtextures; i++)
    {
        /* TODO Do not compute layers fully covered */
        /* TODO Optimize : each layer computes the same shadows */
        tex_color = texturesGetLayerColor(definition->textures + i, renderer, location, detail);
        if (tex_color.a > 0.0001)
        {
            colorMask(&result, &tex_color);
        }
    }

    return result;
}
