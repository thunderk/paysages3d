#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "shared/types.h"
#include "shared/functions.h"
#include "shared/constants.h"
#include "shared/globals.h"

#include "textures.h"
#include "terrain.h"
#include "lighting.h"

static TextureLayerDefinition _NULL_LAYER;

void texturesInit()
{
    _NULL_LAYER = texturesLayerCreateDefinition();
}

void texturesSave(FILE* f, TexturesDefinition* definition)
{
    int i;

    toolsSaveInt(f, definition->nbtextures);
    for (i = 0; i < definition->nbtextures; i++)
    {
        zoneSave(definition->textures[i].zone, f);
        noiseSave(definition->textures[i].bump_noise, f);
        colorSave(definition->textures[i].color, f);
    }
}

void texturesLoad(FILE* f, TexturesDefinition* definition)
{
    TextureLayerDefinition* layer;
    int i, n;

    while (definition->nbtextures > 0)
    {
        texturesDeleteLayer(definition, 0);
    }

    n = toolsLoadInt(f);
    for (i = 0; i < n; i++)
    {
        layer = definition->textures + texturesAddLayer(definition);

        zoneLoad(layer->zone, f);
        noiseLoad(layer->bump_noise, f);
        layer->color = colorLoad(f);
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
    result.color = COLOR_GREEN;

    return result;
}

void texturesLayerDeleteDefinition(TextureLayerDefinition* definition)
{
    zoneDelete(definition->zone);
    noiseDeleteGenerator(definition->bump_noise);
}

void texturesLayerCopyDefinition(TextureLayerDefinition* source, TextureLayerDefinition* destination)
{
    destination->color = source->color;
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
    point.y = renderer->getTerrainHeight(renderer, point.x, point.z) + noiseGet2DTotal(definition->bump_noise, point.x, point.z);

    dpoint.x = point.x - scale;
    dpoint.z = point.z;
    dpoint.y = renderer->getTerrainHeight(renderer, dpoint.x, dpoint.z) + noiseGet2DTotal(definition->bump_noise, dpoint.x, dpoint.z);
    ref.z = -1.0;
    normal = v3Normalize(v3Cross(ref, v3Sub(dpoint, point)));

    dpoint.x = point.x + scale;
    dpoint.z = point.z;
    dpoint.y = renderer->getTerrainHeight(renderer, dpoint.x, dpoint.z) + noiseGet2DTotal(definition->bump_noise, dpoint.x, dpoint.z);
    ref.z = 1.0;
    normal = v3Add(normal, v3Normalize(v3Cross(ref, v3Sub(dpoint, point))));

    ref.z = 0.0;

    dpoint.x = point.x;
    dpoint.z = point.z - scale;
    dpoint.y = renderer->getTerrainHeight(renderer, dpoint.x, dpoint.z) + noiseGet2DTotal(definition->bump_noise, dpoint.x, dpoint.z);
    ref.x = 1.0;
    normal = v3Add(normal, v3Normalize(v3Cross(ref, v3Sub(dpoint, point))));

    dpoint.x = point.x;
    dpoint.z = point.z + scale;
    dpoint.y = renderer->getTerrainHeight(renderer, dpoint.x, dpoint.z) + noiseGet2DTotal(definition->bump_noise, dpoint.x, dpoint.z);
    ref.x = -1.0;
    normal = v3Add(normal, v3Normalize(v3Cross(ref, v3Sub(dpoint, point))));

    return v3Normalize(normal);
}

Color texturesGetLayerColor(TextureLayerDefinition* definition, Renderer* renderer, Vector3 location, double detail)
{
    Color result;
    Vector3 normal;
    double coverage;
    SurfaceMaterial material;

    result = COLOR_TRANSPARENT;
    normal = _getNormal(definition, renderer, location, detail);

    coverage = zoneGetValue(definition->zone, location, normal);
    if (coverage > 0.0)
    {
        material.base = definition->color;
        material.reflection = 0.1;
        material.shininess = 4.0;

        result = renderer->applyLightingToSurface(renderer, location, normal, material);
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
