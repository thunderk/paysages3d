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

#define TEXTURES_MAX 50
static TextureQuality _quality;
static TextureEnvironment _environment;
static int _textures_count = 0;
static TextureDefinition _textures[TEXTURES_MAX];

void texturesInit()
{
    _textures_count = 0;
}

void texturesSave(FILE* f)
{
    int i;

    toolsSaveInt(f, _textures_count);
    for (i = 0; i < _textures_count; i++)
    {
        zoneSave(_textures[i].zone, f);
        noiseSave(_textures[i].bump_noise, f);
        colorSave(_textures[i].color, f);
    }
}

void texturesLoad(FILE* f)
{
    int i, n;
    TextureDefinition* texture;

    while (_textures_count > 0)
    {
        texturesDeleteLayer(0);
    }

    n = toolsLoadInt(f);
    for (i = 0; i < n; i++)
    {
        texture = _textures + texturesAddLayer();

        zoneLoad(texture->zone, f);
        noiseLoad(texture->bump_noise, f);
        texture->color = colorLoad(f);
    }
}

int texturesGetLayerCount()
{
    return _textures_count;
}

int texturesAddLayer()
{
    if (_textures_count < TEXTURES_MAX)
    {
        _textures[_textures_count] = texturesCreateDefinition();

        return _textures_count++;
    }
    else
    {
        return -1;
    }
}

void texturesDeleteLayer(int layer)
{
    if (layer >= 0 && layer < _textures_count)
    {
        zoneDelete(_textures[layer].zone);
        noiseDeleteGenerator(_textures[layer].bump_noise);
        if (_textures_count > 1 && layer < _textures_count - 1)
        {
            memmove(_textures + layer, _textures + layer + 1, sizeof(TextureDefinition) * (_textures_count - layer - 1));
        }
        _textures_count--;
    }
}

TextureDefinition texturesCreateDefinition()
{
    TextureDefinition result;

    result.zone = zoneCreate();
    result.bump_noise = noiseCreateGenerator();
    result.color = COLOR_GREEN;

    return result;
}

void texturesDeleteDefinition(TextureDefinition definition)
{
    zoneDelete(definition.zone);
    noiseDeleteGenerator(definition.bump_noise);
}

void texturesCopyDefinition(TextureDefinition source, TextureDefinition* destination)
{
    destination->color = source.color;
    noiseCopy(source.bump_noise, destination->bump_noise);
    zoneCopy(source.zone, destination->zone);
}

void texturesSetDefinition(int layer, TextureDefinition definition)
{
    TextureDefinition* destination;

    if (layer >= 0 && layer < _textures_count)
    {
        destination = _textures + layer;
        texturesCopyDefinition(definition, destination);
    }
}

TextureDefinition texturesGetDefinition(int layer)
{
    assert(layer >= 0);
    assert(layer < _textures_count);

    return _textures[layer];
}

void texturesSetQuality(TextureQuality quality)
{
    _quality = quality;
}

TextureQuality texturesGetQuality()
{
    return _quality;
}

static inline Vector3 _getNormal(TextureDefinition* definition, Vector3 point, double scale)
{
    Vector3 dpoint, ref, normal;

    ref.x = 0.0;
    ref.y = 0.0;
    point.y = terrainGetHeight(point.x, point.z) + noiseGet2DTotal(definition->bump_noise, point.x, point.z);

    dpoint.x = point.x - scale;
    dpoint.z = point.z;
    dpoint.y = terrainGetHeight(dpoint.x, dpoint.z) + noiseGet2DTotal(definition->bump_noise, dpoint.x, dpoint.z);
    ref.z = -1.0;
    normal = v3Normalize(v3Cross(ref, v3Sub(dpoint, point)));

    dpoint.x = point.x + scale;
    dpoint.z = point.z;
    dpoint.y = terrainGetHeight(dpoint.x, dpoint.z) + noiseGet2DTotal(definition->bump_noise, dpoint.x, dpoint.z);
    ref.z = 1.0;
    normal = v3Add(normal, v3Normalize(v3Cross(ref, v3Sub(dpoint, point))));

    ref.z = 0.0;

    dpoint.x = point.x;
    dpoint.z = point.z - scale;
    dpoint.y = terrainGetHeight(dpoint.x, dpoint.z) + noiseGet2DTotal(definition->bump_noise, dpoint.x, dpoint.z);
    ref.x = 1.0;
    normal = v3Add(normal, v3Normalize(v3Cross(ref, v3Sub(dpoint, point))));

    dpoint.x = point.x;
    dpoint.z = point.z + scale;
    dpoint.y = terrainGetHeight(dpoint.x, dpoint.z) + noiseGet2DTotal(definition->bump_noise, dpoint.x, dpoint.z);
    ref.x = -1.0;
    normal = v3Add(normal, v3Normalize(v3Cross(ref, v3Sub(dpoint, point))));

    return v3Normalize(normal);
}

Color texturesGetLayerColorCustom(Vector3 location, double shadowing, double detail, TextureDefinition* definition, TextureQuality* quality, TextureEnvironment* environment)
{
    Color result;
    Vector3 normal;
    double coverage;

    result.a = 0.0;
    normal = _getNormal(definition, location, detail * 0.3);

    coverage = zoneGetValue(definition->zone, location, normal);
    if (coverage > 0.0)
    {
        result = lightingApply(location, normal, shadowing, definition->color, 0.1, 0.1);
        result.a = coverage;
    }
    return result;
}

Color texturesGetColorCustom(Vector3 location, double shadowing, double detail, TextureQuality* quality, TextureEnvironment* environment)
{
    Color result, tex_color;
    int i;

    result = COLOR_GREEN;
    for (i = 0; i < _textures_count; i++)
    {
        /* TODO Do not compute layers fully covered */
        tex_color = texturesGetLayerColorCustom(location, shadowing, detail, _textures + i, quality, environment);
        if (tex_color.a > 0.0001)
        {
            colorMask(&result, &tex_color);
        }
    }

    return result;
}

Color texturesGetColor(Vector3 location)
{
    double shadowing;

    /* TODO Use environment to get lights to apply */
    shadowing = terrainGetShadow(location, sun_direction_inv);

    return texturesGetColorCustom(location, shadowing, renderGetPrecision(location), &_quality, &_environment);
}
