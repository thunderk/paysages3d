#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "shared/types.h"
#include "shared/functions.h"
#include "shared/globals.h"
#include "shared/constants.h"

#include "textures.h"
#include "water.h"
#include "terrain.h"

static TerrainDefinition _definition;
static TerrainQuality _quality;
static TerrainEnvironment _environment;
static double _max_height = 1.0;

void terrainInit()
{
    _definition = terrainCreateDefinition();
    _max_height = noiseGetMaxValue(_definition.height_noise);
}

void terrainSave(FILE* f)
{
    int i;
    
    noiseSave(_definition.height_noise, f);
    toolsSaveInt(f, _definition.height_modifiers_count);
    for (i = 0; i < _definition.height_modifiers_count; i++)
    {
        modifierSave(_definition.height_modifiers[i], f);
    }
}

void terrainLoad(FILE* f)
{
    int i;
    
    noiseLoad(_definition.height_noise, f);
    _max_height = noiseGetMaxValue(_definition.height_noise);

    for (i = 0; i < _definition.height_modifiers_count; i++)
    {
        modifierDelete(_definition.height_modifiers[i]);
    }
    _definition.height_modifiers_count = toolsLoadInt(f);
    for (i = 0; i < _definition.height_modifiers_count; i++)
    {
        _definition.height_modifiers[i] = modifierCreate();
        modifierLoad(_definition.height_modifiers[i], f);
    }
}

TerrainDefinition terrainCreateDefinition()
{
    TerrainDefinition definition;
    
    definition.height_noise = noiseCreateGenerator();
    definition.height_modifiers_count = 0;
     
    return definition;
}

void terrainDeleteDefinition(TerrainDefinition definition)
{
    int i;
    
    noiseDeleteGenerator(definition.height_noise);
    for (i = 0; i < definition.height_modifiers_count; i++)
    {
        modifierDelete(definition.height_modifiers[i]);
    }
}

void terrainCopyDefinition(TerrainDefinition source, TerrainDefinition* destination)
{
    int i;
    
    noiseCopy(source.height_noise, destination->height_noise);
    
    for (i = 0; i < destination->height_modifiers_count; i++)
    {
        modifierDelete(destination->height_modifiers[i]);
    }
    destination->height_modifiers_count = source.height_modifiers_count;
    for (i = 0; i < destination->height_modifiers_count; i++)
    {
        destination->height_modifiers[i] = modifierCreateCopy(source.height_modifiers[i]);
    }
}

void terrainSetDefinition(TerrainDefinition definition)
{
    terrainCopyDefinition(definition, &_definition);
    _max_height = noiseGetMaxValue(_definition.height_noise);
    /* FIXME _max_height depends on modifiers */
}

TerrainDefinition terrainGetDefinition()
{
    return _definition;
}

int terrainAddModifier(TerrainDefinition* definition, HeightModifier* modifier)
{
    if (definition->height_modifiers_count < MAX_HEIGHT_MODIFIER_COUNT)
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
        /* TODO */
    }
}

void terrainSetQuality(TerrainQuality quality)
{
    _quality = quality;
}

TerrainQuality terrainGetQuality()
{
    return _quality;
}

static inline double _getHeight(TerrainDefinition* definition, double x, double z, double detail)
{
    Vector3 location;
    int i;
    
    location.x = x;
    location.y = noiseGet2DDetail(definition->height_noise, x, z, detail);
    location.z = z;
    
    for (i = 0; i < definition->height_modifiers_count; i++)
    {
        location = modifierApply(definition->height_modifiers[i], location);
    }
    
    return location.y;
}

static inline Vector3 _getPoint(TerrainDefinition* definition, double x, double z, double detail)
{
    Vector3 result;

    result.x = x;
    result.y = _getHeight(definition, x, z, detail);
    result.z = z;

    return result;
}

double terrainGetShadow(Vector3 start, Vector3 direction)
{
    Vector3 inc_vector;
    double inc_value, inc_base, inc_factor, height, diff, light, smoothing, length, water;

    direction = v3Normalize(direction);
    inc_factor = (double)render_quality;
    inc_base = 1.0;
    inc_value = inc_base / inc_factor;
    smoothing = 0.03 * inc_factor;;

    water = waterGetLightFactor(start);

    light = 1.0;
    length = 0.0;
    do
    {
        inc_vector = v3Scale(direction, inc_value);
        length += v3Norm(inc_vector);
        start = v3Add(start, inc_vector);
        height = _getHeight(&_definition, start.x, start.z, inc_value);
        diff = start.y - height;
        if (diff < 0.0)
        {
            light += diff / smoothing;
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
    } while (light > 0.0 && length < 50.0 && start.y <= _max_height);

    light *= water;
    if (light < 0.0)
    {
        return 1.0;
    }
    else
    {
        return 1.0 - light;
    }
}

static Color _getColor(TerrainDefinition* definition, Vector3 point, double precision)
{
    Color color;

    /* FIXME Environment for textures should be customized */
    color = texturesGetColor(point);
    color = fogApplyToLocation(point, color);
    //color = cloudsApplySegmentResult(color, camera_location, point);

    return color;
}

int terrainProjectRay(Vector3 start, Vector3 direction, Vector3* hit_point, Color* hit_color)
{
    Vector3 inc_vector;
    double inc_value, inc_base, inc_factor, height, diff, length;

    direction = v3Normalize(direction);
    inc_factor = (double)render_quality;
    inc_base = 1.0;
    inc_value = inc_base / inc_factor;

    length = 0.0;
    do
    {
        inc_vector = v3Scale(direction, inc_value);
        length += v3Norm(inc_vector);
        start = v3Add(start, inc_vector);
        height = _getHeight(&_definition, start.x, start.z, inc_value);
        diff = start.y - height;
        if (diff < 0.0)
        {
            start.y = height;
            *hit_point = start;
            *hit_color = _getColor(&_definition, start, inc_value);
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
    } while (length < 50.0 && start.y <= _max_height);

    return 0;
}

static int _postProcessFragment(RenderFragment* fragment)
{
    Vector3 point;
    double precision;

    point = fragment->vertex.location;
    precision = renderGetPrecision(point);

    point = _getPoint(&_definition, point.x, point.z, precision);

    fragment->vertex.color = _getColor(&_definition, point, precision);

    return 1;
}

static Vertex _getFirstPassVertex(double x, double z, double detail)
{
    Vertex result;
    double value;

    result.location = _getPoint(&_definition, x, z, 0.0);
    value = sin(x) * sin(x) * cos(z) * cos(z);
    result.color.r = value;
    result.color.g = value;
    result.color.b = value;
    result.color.a = 1.0;
    result.normal.x = result.normal.y = result.normal.z = 0.0;
    result.callback = _postProcessFragment;

    return result;
}

static void _renderQuad(double x, double z, double size)
{
    Vertex v1, v2, v3, v4;

    v1 = _getFirstPassVertex(x, z, size);
    v2 = _getFirstPassVertex(x, z + size, size);
    v3 = _getFirstPassVertex(x + size, z + size, size);
    v4 = _getFirstPassVertex(x + size, z, size);
    renderPushQuad(&v1, &v2, &v3, &v4);
}

double terrainGetHeight(double x, double z)
{
    return _getHeight(&_definition, x, z, 0.0);
}

double terrainGetHeightNormalized(double x, double z)
{
    return 0.5 + _getHeight(&_definition, x, z, 0.0) / (_max_height * 2.0);
}

Color terrainGetColorCustom(double x, double z, double detail, TerrainDefinition* definition, TerrainQuality* quality, TerrainEnvironment* environment)
{
    return _getColor(definition, _getPoint(definition, x, z, detail), detail);
}

Color terrainGetColor(double x, double z, double detail)
{
    return terrainGetColorCustom(x, z, detail, &_definition, &_quality, &_environment);
}

void terrainRender(RenderProgressCallback callback)
{
    int chunk_factor, chunk_count, i;
    double cx = camera_location.x;
    double cz = camera_location.z;
    double radius_int, radius_ext, chunk_size;

    chunk_factor = 1;
    chunk_count = 2;
    radius_int = 0.0;
    radius_ext = _quality.min_chunk_size;
    chunk_size = _quality.min_chunk_size;

    while (radius_ext < 1000.0)
    {
        if (!callback(radius_ext / 1000.0))
        {
            return;
        }
        
        for (i = 0; i < chunk_count - 1; i++)
        {
            _renderQuad(cx - radius_ext + chunk_size * i, cz - radius_ext, chunk_size);
            _renderQuad(cx + radius_int, cz - radius_ext + chunk_size * i, chunk_size);
            _renderQuad(cx + radius_int - chunk_size * i, cz + radius_int, chunk_size);
            _renderQuad(cx - radius_ext, cz + radius_int - chunk_size * i, chunk_size);
        }

        if (chunk_count % 64 == 0 && chunk_size / radius_int < _quality.visible_chunk_size)
        {
            chunk_count /= 2;
            chunk_factor *= 2;
            /* TODO Fill in gaps with triangles */
        }
        chunk_count += 2;
        chunk_size = _quality.min_chunk_size * chunk_factor;
        radius_int = radius_ext;
        radius_ext += chunk_size;
    }
}
