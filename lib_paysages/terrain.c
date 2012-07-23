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
#include "layers.h"
#include "terraincanvas.h"

void terrainSave(PackStream* stream, TerrainDefinition* definition)
{
    noiseSaveGenerator(stream, definition->height_noise);
    packWriteDouble(stream, &definition->height_factor);
    packWriteDouble(stream, &definition->scaling);
    layersSave(stream, definition->canvases);
    packWriteDouble(stream, &definition->shadow_smoothing);
}

void terrainLoad(PackStream* stream, TerrainDefinition* definition)
{
    noiseLoadGenerator(stream, definition->height_noise);
    packReadDouble(stream, &definition->height_factor);
    packReadDouble(stream, &definition->scaling);
    layersLoad(stream, definition->canvases);
    packReadDouble(stream, &definition->shadow_smoothing);

    terrainValidateDefinition(definition);
}

TerrainDefinition terrainCreateDefinition()
{
    TerrainDefinition definition;

    definition.height_noise = noiseCreateGenerator();
    definition.height_factor = 0.0;
    definition.scaling = 1.0;
    definition.canvases = layersCreate(terrainCanvasGetLayerType(), 50);
    definition.shadow_smoothing = 0.0;

    terrainValidateDefinition(&definition);

    return definition;
}

void terrainDeleteDefinition(TerrainDefinition* definition)
{
    noiseDeleteGenerator(definition->height_noise);
    layersDelete(definition->canvases);
}

void terrainCopyDefinition(TerrainDefinition* source, TerrainDefinition* destination)
{
    noiseCopy(source->height_noise, destination->height_noise);
    destination->height_factor = source->height_factor;
    destination->scaling = source->scaling;
    layersCopy(source->canvases, destination->canvases);
    destination->shadow_smoothing = source->shadow_smoothing;

    terrainValidateDefinition(destination);
}

void terrainValidateDefinition(TerrainDefinition* definition)
{
    int i, n;
    TerrainCanvas* canvas;
    double min, max;
    
    noiseValidate(definition->height_noise);
    layersValidate(definition->canvases);
    
    /* Get minimal and maximal height */
    definition->_min_height = -noiseGetMaxValue(definition->height_noise) * definition->height_factor;
    definition->_max_height = noiseGetMaxValue(definition->height_noise) * definition->height_factor;
    
    n = layersCount(definition->canvases);
    for (i = 0; i < n; i++)
    {
        canvas = layersGetLayer(definition->canvases, i);
        terrainCanvasGetLimits(canvas, &min, &max);
        if (min < definition->_min_height)
        {
            definition->_min_height = min;
        }
        if (max > definition->_max_height)
        {
            definition->_max_height = max;
        }
    }
}

static inline double _getHeight(TerrainDefinition* definition, double x, double z)
{
    TerrainCanvas* canvas;
    Vector3 location;
    int i, n;

    location.x = x;
    location.y = noiseGet2DTotal(definition->height_noise, x / definition->scaling, z / definition->scaling) * definition->height_factor;
    location.z = z;
    
    n = layersCount(definition->canvases);
    for (i = 0; i < n; i++)
    {
        canvas = layersGetLayer(definition->canvases, i);
        location = terrainCanvasApply(canvas, location);
    }

    /* TODO Apply modifiers */

    return location.y;
}

static inline double _getHeightDetail(TerrainDefinition* definition, double x, double z, double detail)
{
    TerrainCanvas* canvas;
    Vector3 location;
    int i, n;

    location.x = x;
    location.y = noiseGet2DDetail(definition->height_noise, x / definition->scaling, z / definition->scaling, detail / definition->height_factor) * definition->height_factor;
    location.z = z;

    n = layersCount(definition->canvases);
    for (i = 0; i < n; i++)
    {
        canvas = layersGetLayer(definition->canvases, i);
        location = terrainCanvasApply(canvas, location);
    }

    /* TODO Apply modifiers */

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
    if ((fabs(direction_to_light.x) < 0.0001 && fabs(direction_to_light.z) < 0.0001) || definition->height_factor < 0.001)
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
    inc_base = definition->height_factor / definition->scaling;
    inc_value = inc_base / inc_factor;
    smoothing = definition->shadow_smoothing;

    light_factor = 1.0;
    length = 0.0;
    diff = 0.0;
    do
    {
        inc_vector = v3Scale(direction_to_light, inc_value);
        length += v3Norm(inc_vector);
        location = v3Add(location, inc_vector);
        height = _getHeightDetail(definition, location.x, location.z, diff / inc_factor);
        diff = location.y - height;
        if (diff < 0.0)
        {
            if (length * smoothing > 0.000001)
            {
                light_factor += diff * v3Norm(inc_vector) / (length * smoothing);
            }
            else
            {
                light_factor = 0.0;
            }
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
    } while (light_factor > 0.0 && length < (10.0 * inc_factor) && location.y <= definition->_max_height);

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

static Color _postProcessFragment(Renderer* renderer, Vector3 point, void* data)
{
    double precision;
    TerrainDefinition* definition;

    definition = (TerrainDefinition*)data;

    point = _getPoint(definition, point.x, point.z);

    precision = renderer->getPrecision(renderer, point);
    return _getColor(definition, renderer, point, precision);
}

static void _renderQuad(TerrainDefinition* definition, Renderer* renderer, double x, double z, double size, double water_height)
{
    Vector3 v1, v2, v3, v4;

    v1 = _getPoint(definition, x, z);
    v2 = _getPoint(definition, x, z + size);
    v3 = _getPoint(definition, x + size, z + size);
    v4 = _getPoint(definition, x + size, z);
    
    if (v1.y > water_height || v2.y > water_height || v3.y > water_height || v4.y > water_height)
    {
        renderer->pushQuad(renderer, v1, v2, v3, v4, _postProcessFragment, definition);
    }
}

double terrainGetHeight(TerrainDefinition* definition, double x, double z)
{
    return _getHeight(definition, x, z);
}

double terrainGetHeightNormalized(TerrainDefinition* definition, double x, double z)
{
    if (definition->_max_height - definition->_min_height <= 0.0000001)
    {
        return 0.5;
    }
    else
    {
        return (_getHeight(definition, x, z) - definition->_min_height) / (definition->_max_height - definition->_min_height);
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
