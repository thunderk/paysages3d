#include "private.h"

#include <stdlib.h>
#include <math.h>
#include "../tools.h"
#include "../renderer.h"

/******************** Definition ********************/
static void _validateDefinition(TerrainDefinition* definition)
{
    noiseValidate(definition->_height_noise);

    /* Get minimal and maximal height */
    definition->_min_height = -noiseGetMaxValue(definition->_height_noise) * definition->height;
    definition->_max_height = noiseGetMaxValue(definition->_height_noise) * definition->height;
}

static TerrainDefinition* _createDefinition()
{
    TerrainDefinition* definition = malloc(sizeof(TerrainDefinition));

    definition->height = 0.0;
    definition->scaling = 1.0;
    definition->shadow_smoothing = 0.0;

    definition->height_map = terrainHeightMapCreate(definition);

    definition->_height_noise = noiseCreateGenerator();

    terrainAutoPreset(definition, TERRAIN_PRESET_STANDARD);

    return definition;
}

static void _deleteDefinition(TerrainDefinition* definition)
{
    terrainHeightmapDelete(definition->height_map);
    noiseDeleteGenerator(definition->_height_noise);
    free(definition);
}

static void _copyDefinition(TerrainDefinition* source, TerrainDefinition* destination)
{
    destination->height = source->height;
    destination->scaling = source->scaling;
    destination->shadow_smoothing = source->shadow_smoothing;

    terrainHeightmapCopy(source->height_map, destination->height_map);

    noiseCopy(source->_height_noise, destination->_height_noise);

    _validateDefinition(destination);
}

static void _saveDefinition(PackStream* stream, TerrainDefinition* definition)
{
    packWriteDouble(stream, &definition->height);
    packWriteDouble(stream, &definition->scaling);
    packWriteDouble(stream, &definition->shadow_smoothing);
    terrainHeightmapSave(stream, definition->height_map);
    noiseSaveGenerator(stream, definition->_height_noise);
}

static void _loadDefinition(PackStream* stream, TerrainDefinition* definition)
{
    packReadDouble(stream, &definition->height);
    packReadDouble(stream, &definition->scaling);
    packReadDouble(stream, &definition->shadow_smoothing);
    terrainHeightmapLoad(stream, definition->height_map);
    noiseLoadGenerator(stream, definition->_height_noise);

    _validateDefinition(definition);
}

StandardDefinition TerrainDefinitionClass = {
    (FuncObjectCreate)_createDefinition,
    (FuncObjectDelete)_deleteDefinition,
    (FuncObjectCopy)_copyDefinition,
    (FuncObjectValidate)_validateDefinition,
    (FuncObjectSave)_saveDefinition,
    (FuncObjectLoad)_loadDefinition
};

/******************** Binding ********************/
static double _fakeGetHeight(Renderer* renderer, double x, double z, int with_painting)
{
    UNUSED(renderer);
    UNUSED(x);
    UNUSED(z);
    UNUSED(with_painting);

    return 0.0;
}

static double _getHeight(Renderer* renderer, double x, double z, int with_painting)
{
    double height;
    TerrainDefinition* definition = renderer->terrain->definition;
    x /= definition->scaling;
    z /= definition->scaling;

    if (!with_painting || !terrainHeightmapGetHeight(definition->height_map, x, z, &height))
    {
        height = noiseGet2DTotal(definition->_height_noise, x, z);
    }

    return height * definition->height * definition->scaling;
}

static Color _fakeGetFinalColor(Renderer* renderer, Vector3 location, double precision)
{
    return COLOR_GREEN;
}

static Color _getFinalColor(Renderer* renderer, Vector3 location, double precision)
{
    Color color;

    color = renderer->applyTextures(renderer, location, precision);

    /* TODO Factorize this in scenery renderer */
    color = renderer->atmosphere->applyAerialPerspective(renderer, location, color);
    color = renderer->applyClouds(renderer, color, renderer->camera_location, location);

    return color;
}

static RayCastingResult _fakeCastRay(Renderer* renderer, Vector3 start, Vector3 direction)
{
    UNUSED(renderer);
    UNUSED(start);
    UNUSED(direction);

    RayCastingResult result;
    result.hit = 0;
    return result;
}

static RayCastingResult _castRay(Renderer* renderer, Vector3 start, Vector3 direction)
{
    RayCastingResult result;
    TerrainDefinition* definition = renderer->terrain->definition;
    Vector3 inc_vector;
    double inc_value, inc_base, inc_factor, height, diff, lastdiff, length;

    direction = v3Normalize(direction);
    inc_factor = (double)renderer->render_quality;
    inc_base = 1.0;
    inc_value = inc_base / inc_factor;
    lastdiff = start.y - _getHeight(renderer, start.x, start.z, 1);

    length = 0.0;
    do
    {
        inc_vector = v3Scale(direction, inc_value);
        length += v3Norm(inc_vector);
        start = v3Add(start, inc_vector);
        height = _getHeight(renderer, start.x, start.z, 1);
        diff = start.y - height;
        if (diff < 0.0)
        {
            if (fabs(diff - lastdiff) > 0.00001)
            {
                start = v3Add(start, v3Scale(inc_vector, -diff / (diff - lastdiff)));
                start.y = _getHeight(renderer, start.x, start.z, 1);
            }
            else
            {
                start.y = height;
            }
            result.hit = 1;
            result.hit_location = start;
            result.hit_color = _getFinalColor(renderer, start, renderer->getPrecision(renderer, result.hit_location));
            return result;
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

    result.hit = 0;
    return result;
}

static LightDefinition _fakeAlterLight(Renderer* renderer, LightDefinition* light, Vector3 at)
{
    UNUSED(renderer);
    UNUSED(at);

    return *light;
}

static LightDefinition _alterLight(Renderer* renderer, LightDefinition* light, Vector3 location)
{
    TerrainDefinition* definition = renderer->terrain->definition;
    LightDefinition result = *light;
    Vector3 inc_vector, direction_to_light;
    double inc_value, inc_base, inc_factor, height, diff, light_factor, smoothing, length;

    direction_to_light = v3Scale(light->direction, -1.0);
    if ((fabs(direction_to_light.x) < 0.0001 && fabs(direction_to_light.z) < 0.0001) || definition->height < 0.001)
    {
        return result;
    }
    else if (direction_to_light.y < 0.05)
    {
        result.color = COLOR_BLACK;
        return result;
    }
    else if (direction_to_light.y < 0.0000)
    {
        result.color.r *= (0.05 + direction_to_light.y) / 0.05;
        result.color.g *= (0.05 + direction_to_light.y) / 0.05;
        result.color.b *= (0.05 + direction_to_light.y) / 0.05;
    }

    inc_factor = (double)renderer->render_quality;
    inc_base = definition->height / definition->scaling;
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
        height = _getHeight(renderer, location.x, location.z, 1);
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
        result.color = COLOR_BLACK;
        return result;
    }
    else
    {
        result.color.r *= light_factor;
        result.color.g *= light_factor;
        result.color.b *= light_factor;

        return result;
    }
}

/******************** Public tools ********************/
double terrainGetGridHeight(TerrainDefinition* definition, int x, int z, int with_painting)
{
    double height;

    if (!with_painting || !terrainHeightmapGetHeight(definition->height_map, (double)x, (double)z, &height))
    {
        height = noiseGet2DTotal(definition->_height_noise, (double)x, (double)z);
    }

    return height;
}

/******************** Renderer ********************/
static TerrainRenderer* _createRenderer()
{
    TerrainRenderer* result;

    result = malloc(sizeof(TerrainRenderer));
    result->definition = TerrainDefinitionClass.create();

    result->castRay = _fakeCastRay;
    result->getHeight = _fakeGetHeight;
    result->getFinalColor = _fakeGetFinalColor;

    return result;
}

static void _deleteRenderer(TerrainRenderer* renderer)
{
    TerrainDefinitionClass.destroy(renderer->definition);
    free(renderer);
}

static void _bindRenderer(TerrainRenderer* renderer, TerrainDefinition* definition)
{
    TerrainDefinitionClass.copy(definition, renderer->definition);

    renderer->castRay = _castRay;
    renderer->getHeight = _getHeight;
    renderer->getFinalColor = _getFinalColor;
}

StandardRenderer TerrainRendererClass = {
    (FuncObjectCreate)_createRenderer,
    (FuncObjectDelete)_deleteRenderer,
    (FuncObjectBind)_bindRenderer
};
