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
    noiseGetRange(definition->_height_noise, &definition->_min_height, &definition->_max_height);
    definition->_min_height *= definition->height;
    definition->_max_height *= definition->height;
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

static double _realGetHeight(Renderer* renderer, double x, double z, int with_painting)
{
    return terrainGetInterpolatedHeight(renderer->terrain->definition, x, z, with_painting);
}

static TerrainResult _fakeGetResult(Renderer* renderer, double x, double z, int with_painting, int with_textures)
{
    TerrainResult result;

    UNUSED(renderer);
    UNUSED(x);
    UNUSED(z);
    UNUSED(with_painting);
    UNUSED(with_textures);

    result.location.x = x;
    result.location.y = 0.0;
    result.location.z = z;
    result.normal = VECTOR_UP;

    return result;
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

static TerrainResult _realGetResult(Renderer* renderer, double x, double z, int with_painting, int with_textures)
{
    TerrainResult result;
    double detail = 0.001; /* TODO */

    /* Normal */
    Vector3 center, north, east, south, west;

    center.x = x;
    center.z = z;
    center.y = renderer->terrain->getHeight(renderer, center.x, center.z, with_painting);

    east.x = x + detail;
    east.z = z;
    east.y = renderer->terrain->getHeight(renderer, east.x, east.z, with_painting);

    south.x = x;
    south.z = z + detail;
    south.y = renderer->terrain->getHeight(renderer, south.x, south.z, with_painting);

    if (renderer->render_quality > 6)
    {
        west.x = x - detail;
        west.z = z;
        west.y = renderer->terrain->getHeight(renderer, west.x, west.z, with_painting);

        north.x = x;
        north.z = z - detail;
        north.y = renderer->terrain->getHeight(renderer, north.x, north.z, with_painting);

        result.normal = _getNormal4(center, north, east, south, west);
    }
    else
    {
        result.normal = _getNormal2(center, east, south);
    }

    /* Location */
    result.location = center;

    /* Texture displacement */
    if (with_textures)
    {
        center = renderer->textures->displaceTerrain(renderer, result);
        result.location = center;

        /* Recompute normal */
        if (renderer->render_quality > 6)
        {
            /* Use 5 points on displaced terrain */
            east = renderer->textures->displaceTerrain(renderer, _realGetResult(renderer, east.x, east.z, with_painting, 0));
            south = renderer->textures->displaceTerrain(renderer, _realGetResult(renderer, south.x, south.z, with_painting, 0));
            west = renderer->textures->displaceTerrain(renderer, _realGetResult(renderer, west.x, west.z, with_painting, 0));
            north = renderer->textures->displaceTerrain(renderer, _realGetResult(renderer, north.x, north.z, with_painting, 0));

            result.normal = _getNormal4(center, north, east, south, west);
        }
        else if (renderer->render_quality > 2)
        {
            /* Use 3 points on displaced terrain */
            east = renderer->textures->displaceTerrain(renderer, _realGetResult(renderer, east.x, east.z, with_painting, 0));
            south = renderer->textures->displaceTerrain(renderer, _realGetResult(renderer, south.x, south.z, with_painting, 0));

            result.normal = _getNormal2(center, east, south);
        }
        else
        {
            /* TODO Use texture noise directly, as if terrain was a plane */
        }
    }

    return result;
}

static Color _fakeGetFinalColor(Renderer* renderer, Vector3 location, double precision)
{
    UNUSED(renderer);
    UNUSED(location);
    UNUSED(precision);
    return COLOR_GREEN;
}

static Color _realGetFinalColor(Renderer* renderer, Vector3 location, double precision)
{
    /* TODO Restore precision control */
    TexturesResult textures = renderer->textures->applyToTerrain(renderer, location.x, location.z);
    return renderer->applyMediumTraversal(renderer, textures.final_location, textures.final_color);
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

static RayCastingResult _realCastRay(Renderer* renderer, Vector3 start, Vector3 direction)
{
    RayCastingResult result;
    TerrainDefinition* definition = renderer->terrain->definition;
    Vector3 inc_vector;
    double inc_value, inc_base, inc_factor, height, diff, lastdiff, length;

    direction = v3Normalize(direction);
    inc_factor = (double)renderer->render_quality;
    inc_base = 1.0;
    inc_value = inc_base / inc_factor;
    lastdiff = start.y - _realGetHeight(renderer, start.x, start.z, 1);

    length = 0.0;
    do
    {
        inc_vector = v3Scale(direction, inc_value);
        length += v3Norm(inc_vector);
        start = v3Add(start, inc_vector);
        height = _realGetHeight(renderer, start.x, start.z, 1);
        diff = start.y - height;
        if (diff < 0.0)
        {
            if (fabs(diff - lastdiff) > 0.00001)
            {
                start = v3Add(start, v3Scale(inc_vector, -diff / (diff - lastdiff)));
                start.y = _realGetHeight(renderer, start.x, start.z, 1);
            }
            else
            {
                start.y = height;
            }
            result.hit = 1;
            result.hit_location = start;
            result.hit_color = _realGetFinalColor(renderer, start, renderer->getPrecision(renderer, result.hit_location));
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

static int _alterLight(Renderer* renderer, LightDefinition* light, Vector3 location)
{
    TerrainDefinition* definition = renderer->terrain->definition;
    Vector3 inc_vector, direction_to_light;
    double inc_value, inc_base, inc_factor, height, diff, light_factor, smoothing, length;

    direction_to_light = v3Scale(light->direction, -1.0);
    if (direction_to_light.y < -0.05)
    {
        light->color = COLOR_BLACK;
        return 1;
    }
    else if (direction_to_light.y < 0.0000)
    {
        light->color.r *= (0.05 + direction_to_light.y) / 0.05;
        light->color.g *= (0.05 + direction_to_light.y) / 0.05;
        light->color.b *= (0.05 + direction_to_light.y) / 0.05;
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
        height = renderer->terrain->getResult(renderer, location.x, location.z, 1, 1).location.y;
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
        light->color = COLOR_BLACK;
        return 1;
    }
    else
    {
        light->color.r *= light_factor;
        light->color.g *= light_factor;
        light->color.b *= light_factor;

        return 1;
    }
}

/******************** Public tools ********************/
double terrainGetGridHeight(TerrainDefinition* definition, int x, int z, int with_painting)
{
    double height;

    if (!with_painting || !terrainHeightmapGetGridHeight(definition->height_map, x, z, &height))
    {
        height = noiseGet2DTotal(definition->_height_noise, (double)x, (double)z);
    }

    return height;
}

double terrainGetInterpolatedHeight(TerrainDefinition* definition, double x, double z, int with_painting)
{
    double height;
    x /= definition->scaling;
    z /= definition->scaling;

    if (!with_painting || !terrainHeightmapGetInterpolatedHeight(definition->height_map, x, z, &height))
    {
        height = noiseGet2DTotal(definition->_height_noise, x, z);
    }

    return height * definition->height * definition->scaling;
}

/******************** Renderer ********************/
static TerrainRenderer* _createRenderer()
{
    TerrainRenderer* result;

    result = malloc(sizeof(TerrainRenderer));
    result->definition = TerrainDefinitionClass.create();

    result->castRay = _fakeCastRay;
    result->getHeight = _fakeGetHeight;
    result->getResult = _fakeGetResult;
    result->getFinalColor = _fakeGetFinalColor;

    return result;
}

static void _deleteRenderer(TerrainRenderer* renderer)
{
    TerrainDefinitionClass.destroy(renderer->definition);
    free(renderer);
}

static void _bindRenderer(Renderer* renderer, TerrainDefinition* definition)
{
    TerrainDefinitionClass.copy(definition, renderer->terrain->definition);

    renderer->terrain->castRay = _realCastRay;
    renderer->terrain->getHeight = _realGetHeight;
    renderer->terrain->getResult = _realGetResult;
    renderer->terrain->getFinalColor = _realGetFinalColor;

    lightingManagerRegisterFilter(renderer->lighting, (FuncLightingAlterLight)_alterLight, renderer);
}

StandardRenderer TerrainRendererClass = {
    (FuncObjectCreate)_createRenderer,
    (FuncObjectDelete)_deleteRenderer,
    (FuncObjectBind)_bindRenderer
};
