#include "terrain.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "shared/types.h"
#include "euclid.h"
#include "render.h"
#include "textures.h"
#include "water.h"
#include "tools.h"

void terrainInit()
{
}

void terrainQuit()
{
}

void terrainSave(PackStream* stream, TerrainDefinition* definition)
{
    int i;

    noiseSaveGenerator(stream, definition->height_noise);
    packWriteDouble(stream, &definition->height_factor);
    packWriteDouble(stream, &definition->scaling);

    packWriteInt(stream, &definition->height_modifiers_count);
    for (i = 0; i < definition->height_modifiers_count; i++)
    {
        modifierSave(stream, definition->height_modifiers[i]);
    }
}

void terrainLoad(PackStream* stream, TerrainDefinition* definition)
{
    int i, n;
    HeightModifier* modifier;

    noiseLoadGenerator(stream, definition->height_noise);
    packReadDouble(stream, &definition->height_factor);
    packReadDouble(stream, &definition->scaling);

    while (definition->height_modifiers_count > 0)
    {
        terrainDelModifier(definition, 0);
    }
    packReadInt(stream, &n);
    for (i = 0; i < n; i++)
    {
        modifier = modifierCreate();
        modifierLoad(stream, modifier);
        terrainAddModifier(definition, modifier);
        modifierDelete(modifier);
    }

    terrainValidateDefinition(definition);
}

TerrainDefinition terrainCreateDefinition()
{
    TerrainDefinition definition;

    definition.height_noise = noiseCreateGenerator();
    definition.height_factor = 0.0;
    definition.scaling = 1.0;
    definition.height_modifiers_count = 0;

    terrainValidateDefinition(&definition);

    return definition;
}

void terrainDeleteDefinition(TerrainDefinition* definition)
{
    int i;

    noiseDeleteGenerator(definition->height_noise);
    for (i = 0; i < definition->height_modifiers_count; i++)
    {
        modifierDelete(definition->height_modifiers[i]);
    }
}

void terrainCopyDefinition(TerrainDefinition* source, TerrainDefinition* destination)
{
    int i;

    noiseCopy(source->height_noise, destination->height_noise);
    destination->height_factor = source->height_factor;
    destination->scaling = source->scaling;

    for (i = 0; i < destination->height_modifiers_count; i++)
    {
        modifierDelete(destination->height_modifiers[i]);
    }
    destination->height_modifiers_count = 0;

    for (i = 0; i < source->height_modifiers_count; i++)
    {
        terrainAddModifier(destination, source->height_modifiers[i]);
    }

    terrainValidateDefinition(destination);
}

void terrainValidateDefinition(TerrainDefinition* definition)
{
    definition->_max_height = noiseGetMaxValue(definition->height_noise) * definition->height_factor;
    /* FIXME _max_height depends on modifiers */
}

int terrainAddModifier(TerrainDefinition* definition, HeightModifier* modifier)
{
    if (definition->height_modifiers_count < TERRAIN_MAX_MODIFIERS)
    {
        definition->height_modifiers[definition->height_modifiers_count] = modifierCreateCopy(modifier);
        return definition->height_modifiers_count++;
    }
    else
    {
        return -1;
    }
}

void terrainDelModifier(TerrainDefinition* definition, int modifier_position)
{
    if (modifier_position >= 0 && modifier_position < definition->height_modifiers_count)
    {
        modifierDelete(definition->height_modifiers[modifier_position]);
        if (definition->height_modifiers_count > 1 && modifier_position < definition->height_modifiers_count - 1)
        {
            memmove(definition->height_modifiers + modifier_position, definition->height_modifiers + modifier_position + 1, sizeof(HeightModifier*) * (definition->height_modifiers_count - modifier_position - 1));
        }
        definition->height_modifiers_count--;
    }
}

static inline double _getHeight(TerrainDefinition* definition, double x, double z)
{
    Vector3 location;
    int i;

    location.x = x;
    location.y = noiseGet2DTotal(definition->height_noise, x / definition->scaling, z / definition->scaling) * definition->height_factor;
    location.z = z;

    for (i = 0; i < definition->height_modifiers_count; i++)
    {
        location = modifierApply(definition->height_modifiers[i], location);
    }

    return location.y;
}

static inline Vector3 _getPoint(TerrainDefinition* definition, double x, double z)
{
    Vector3 result;

    result.x = x;
    result.y = _getHeight(definition, x, z);
    result.z = z;

    return result;
}

Color terrainLightFilter(TerrainDefinition* definition, Renderer* renderer, Color light, Vector3 location, Vector3 light_location, Vector3 direction_to_light)
{
    Vector3 inc_vector;
    double inc_value, inc_base, inc_factor, height, diff, light_factor, smoothing, length;

    direction_to_light = v3Normalize(direction_to_light);
    if (fabs(direction_to_light.x) < 0.0001 && fabs(direction_to_light.z) < 0.0001)
    {
        return light;
    }
    else if (direction_to_light.y < 0.05)
    {
        return COLOR_BLACK;
    }
    else if (direction_to_light.y < 0.0000)
    {
        light.r *= (0.05 + direction_to_light.y) / 0.05;
        light.g *= (0.05 + direction_to_light.y) / 0.05;
        light.b *= (0.05 + direction_to_light.y) / 0.05;
    }

    inc_factor = (double)renderer->render_quality;
    inc_base = 1.0;
    inc_value = inc_base / inc_factor;
    smoothing = 0.03 * inc_factor;

    light_factor = 1.0;
    length = 0.0;
    do
    {
        inc_vector = v3Scale(direction_to_light, inc_value);
        length += v3Norm(inc_vector);
        location = v3Add(location, inc_vector);
        height = _getHeight(definition, location.x, location.z);
        diff = location.y - height;
        if (diff < 0.0)
        {
            light_factor += diff / smoothing;
        }

        if (diff < inc_base / inc_factor)
        {
            inc_value = inc_base / inc_factor;
        }
        else if (diff > inc_base)
        {
            inc_value = inc_base;
        }
        else
        {
            inc_value = diff;
        }
    } while (light_factor > 0.0 && length < 50.0 && location.y <= definition->_max_height);

    if (light_factor <= 0.0)
    {
        return COLOR_BLACK;
    }
    else
    {
        light.r *= light_factor;
        light.g *= light_factor;
        light.b *= light_factor;

        return light;
    }
}

static Color _getColor(TerrainDefinition* definition, Renderer* renderer, Vector3 point, double precision)
{
    Color color;

    color = renderer->applyTextures(renderer, point, precision);
    color = renderer->applyAtmosphere(renderer, point, color);
    color = renderer->applyClouds(renderer, color, renderer->camera_location, point);

    return color;
}

int terrainProjectRay(TerrainDefinition* definition, Renderer* renderer, Vector3 start, Vector3 direction, Vector3* hit_point, Color* hit_color)
{
    Vector3 inc_vector;
    double inc_value, inc_base, inc_factor, height, diff, lastdiff, length;

    direction = v3Normalize(direction);
    inc_factor = (double)renderer->render_quality;
    inc_base = 1.0;
    inc_value = inc_base / inc_factor;
    lastdiff = start.y - _getHeight(definition, start.x, start.z);

    length = 0.0;
    do
    {
        inc_vector = v3Scale(direction, inc_value);
        length += v3Norm(inc_vector);
        start = v3Add(start, inc_vector);
        height = _getHeight(definition, start.x, start.z);
        diff = start.y - height;
        if (diff < 0.0)
        {
            if (fabs(diff - lastdiff) > 0.00001)
            {
                start = v3Add(start, v3Scale(inc_vector, -diff / (diff - lastdiff)));
                start.y = _getHeight(definition, start.x, start.z);
            }
            else
            {
                start.y = height;
            }
            *hit_point = start;
            *hit_color = _getColor(definition, renderer, start, renderer->getPrecision(renderer, start));
            return 1;
        }

        if (diff < inc_base / inc_factor)
        {
            inc_value = inc_base / inc_factor;
        }
        else if (diff > inc_base)
        {
            inc_value = inc_base;
        }
        else
        {
            inc_value = diff;
        }
        lastdiff = diff;
    } while (length < 50.0 && start.y <= definition->_max_height);

    return 0;
}

static int _postProcessFragment(RenderFragment* fragment, Renderer* renderer, void* data)
{
    Vector3 point;
    double precision;
    TerrainDefinition* definition;

    definition = (TerrainDefinition*)data;

    point = fragment->vertex.location;
    point = _getPoint(definition, point.x, point.z);

    precision = renderer->getPrecision(renderer, point);
    fragment->vertex.color = _getColor(definition, renderer, point, precision);

    return 1;
}

static Vertex _getFirstPassVertex(TerrainDefinition* definition, double x, double z, double detail)
{
    Vertex result;
    double value;

    result.location = _getPoint(definition, x, z);
    value = sin(x) * sin(x) * cos(z) * cos(z);
    result.color.r = value;
    result.color.g = value;
    result.color.b = value;
    result.color.a = 1.0;
    result.normal.x = result.normal.y = result.normal.z = 0.0;
    result.callback = _postProcessFragment;
    result.callback_data = definition;

    return result;
}

static void _renderQuad(TerrainDefinition* definition, Renderer* renderer, double x, double z, double size, double water_height)
{
    Vertex v1, v2, v3, v4;

    v1 = _getFirstPassVertex(definition, x, z, size);
    v2 = _getFirstPassVertex(definition, x, z + size, size);
    v3 = _getFirstPassVertex(definition, x + size, z + size, size);
    v4 = _getFirstPassVertex(definition, x + size, z, size);
    
    if (v1.location.y > water_height || v2.location.y > water_height || v3.location.y > water_height || v4.location.y > water_height)
    {
        renderer->pushQuad(renderer, &v1, &v2, &v3, &v4);
    }
}

double terrainGetHeight(TerrainDefinition* definition, double x, double z)
{
    return _getHeight(definition, x, z);
}

double terrainGetHeightNormalized(TerrainDefinition* definition, double x, double z)
{
    if (definition->_max_height == 0.0)
    {
        return 0.5;
    }
    else
    {
        return 0.5 + _getHeight(definition, x, z) / (definition->_max_height * 2.0);
    }
}

Color terrainGetColor(TerrainDefinition* definition, Renderer* renderer, double x, double z, double detail)
{
    Vector3 point = _getPoint(definition, x, z);
    return _getColor(definition, renderer, point, detail);
}

void terrainRender(TerrainDefinition* definition, Renderer* renderer)
{
    int chunk_factor, chunk_count, i;
    double cx = renderer->camera_location.x;
    double cz = renderer->camera_location.z;
    double min_chunk_size, visible_chunk_size;
    double radius_int, radius_ext, chunk_size;
    double water_height;

    min_chunk_size = 0.1 / (double)renderer->render_quality;
    visible_chunk_size = 0.05 / (double)renderer->render_quality;

    chunk_factor = 1;
    chunk_count = 2;
    radius_int = 0.0;
    radius_ext = min_chunk_size;
    chunk_size = min_chunk_size;
    
    water_height = renderer->getWaterHeightInfo(renderer).max_height;

    while (radius_ext < 1000.0)
    {
        if (!renderer->addRenderProgress(renderer, 0.0))
        {
            return;
        }

        for (i = 0; i < chunk_count - 1; i++)
        {
            _renderQuad(definition, renderer, cx - radius_ext + chunk_size * i, cz - radius_ext, chunk_size, water_height);
            _renderQuad(definition, renderer, cx + radius_int, cz - radius_ext + chunk_size * i, chunk_size, water_height);
            _renderQuad(definition, renderer, cx + radius_int - chunk_size * i, cz + radius_int, chunk_size, water_height);
            _renderQuad(definition, renderer, cx - radius_ext, cz + radius_int - chunk_size * i, chunk_size, water_height);
        }

        if (chunk_count % 64 == 0 && chunk_size / radius_int < visible_chunk_size)
        {
            chunk_count /= 2;
            chunk_factor *= 2;
            /* TODO Fill in gaps with triangles */
        }
        chunk_count += 2;
        chunk_size = min_chunk_size * chunk_factor;
        radius_int = radius_ext;
        radius_ext += chunk_size;
    }
}
