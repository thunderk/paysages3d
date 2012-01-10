#include "shared/types.h"
#include "shared/functions.h"
#include "shared/constants.h"
#include "shared/globals.h"

#include "water.h"
#include "terrain.h"

#include <math.h>

static WaterDefinition _definition;
static WaterQuality _quality;
static WaterEnvironment _environment;

static RayCastingResult _reflectionFunction(Vector3 start, Vector3 direction)
{
    RayCastingResult result;

    if (!terrainProjectRay(start, direction, &result.hit_location, &result.hit_color))
    {
        result.hit_color = skyProjectRay(start, direction);
        /* TODO hit_location */
    }

    result.hit = 1;
    return result;
}

static RayCastingResult _refractionFunction(Vector3 start, Vector3 direction)
{
    RayCastingResult result;

    result.hit = terrainProjectRay(start, direction, &result.hit_location, &result.hit_color);

    return result;
}

void waterInit()
{
    _definition = waterCreateDefinition();

    /* TODO quality */

    _environment.reflection_function = _reflectionFunction;
    _environment.refraction_function = _refractionFunction;
    _environment.toggle_fog = 1;
    _environment.toggle_shadows = 1;
}

void waterSave(FILE* f)
{
    toolsSaveDouble(f, _definition.height);
    colorSave(_definition.main_color, f);
    colorSave(_definition.depth_color, f);
    toolsSaveDouble(f, _definition.transparency_depth);
    toolsSaveDouble(f, _definition.transparency);
    toolsSaveDouble(f, _definition.reflection);
    noiseSave(_definition.height_noise, f);
    toolsSaveDouble(f, _definition.height_noise_factor);
}

void waterLoad(FILE* f)
{
    _definition.height = toolsLoadDouble(f);
    _definition.main_color = colorLoad(f);
    _definition.depth_color = colorLoad(f);
    _definition.transparency_depth = toolsLoadDouble(f);
    _definition.transparency = toolsLoadDouble(f);
    _definition.reflection = toolsLoadDouble(f);
    noiseLoad(_definition.height_noise, f);
    _definition.height_noise_factor = toolsLoadDouble(f);
}

WaterDefinition waterCreateDefinition()
{
    WaterDefinition result;

    result.main_color = COLOR_BLACK;
    result.depth_color = COLOR_BLACK;
    result.height = -1000.0;
    result.height_noise = noiseCreateGenerator();
    result.height_noise_factor = 1.0;

    return result;
}

void waterDeleteDefinition(WaterDefinition definition)
{
    noiseDeleteGenerator(definition.height_noise);
}

void waterCopyDefinition(WaterDefinition source, WaterDefinition* destination)
{
    NoiseGenerator* noise;

    noise = destination->height_noise;
    *destination = source;
    destination->height_noise = noise;
    noiseCopy(source.height_noise, destination->height_noise);
}

void waterSetDefinition(WaterDefinition config)
{
    waterCopyDefinition(config, &_definition);
}

WaterDefinition waterGetDefinition()
{
    return _definition;
}

void waterSetQuality(WaterQuality quality)
{
    _quality = quality;

    _quality.detail_boost = (_quality.detail_boost < 0.1) ? 0.1 : _quality.detail_boost;
}

WaterQuality waterGetQuality()
{
    return _quality;
}

static inline double _getHeight(WaterDefinition* definition, double x, double z, double detail)
{
    return definition->height + noiseGet2DDetail(definition->height_noise, x, z, detail) * definition->height_noise_factor;
}

static inline Vector3 _getNormal(WaterDefinition* definition, Vector3 base, double detail)
{
    Vector3 back, right;
    double x, z;

    x = base.x;
    z = base.z;

    back.x = x;
    back.y = _getHeight(definition, x, z + detail, detail);
    back.z = z + detail;
    back = v3Sub(back, base);

    right.x = x + detail;
    right.y = _getHeight(definition, x + detail, z, detail);
    right.z = z;
    right = v3Sub(right, base);

    return v3Normalize(v3Cross(back, right));
}

static inline Vector3 _reflectRay(Vector3 incoming, Vector3 normal)
{
    double c;

    c = v3Dot(normal, v3Scale(incoming, -1.0));
    return v3Add(incoming, v3Scale(normal, 2.0 * c));
}

static inline Vector3 _refractRay(Vector3 incoming, Vector3 normal)
{
    double c1, c2, f;

    f = 1.0 / 1.33;
    c1 = v3Dot(normal, v3Scale(incoming, -1.0));
    c2 = sqrt(1.0 - pow(f, 2.0) * (1.0 - pow(c1, 2.0)));
    if (c1 >= 0.0)
    {
        return v3Add(v3Scale(incoming, f), v3Scale(normal, f * c1 - c2));
    }
    else
    {
        return v3Add(v3Scale(incoming, f), v3Scale(normal, c2 - f * c1));
    }
}

WaterResult waterGetColorCustom(Vector3 location, Vector3 look, WaterDefinition* definition, WaterQuality* quality, WaterEnvironment* environment)
{
    WaterResult result;
    RayCastingResult refracted;
    Vector3 normal;
    Color color;
    double shadowed, detail, depth;

    if (definition == NULL)
    {
        definition = &_definition;
    }
    if (quality == NULL)
    {
        quality = &_quality;
    }
    if (environment == NULL)
    {
        environment = &_environment;
    }

    if (quality->force_detail != 0.0)
    {
        detail = quality->force_detail;
    }
    else
    {
        detail = renderGetPrecision(location) / quality->detail_boost;
    }

    location.y = _getHeight(definition, location.x, location.z, detail);
    result.location = location;

    normal = _getNormal(definition, location, detail);
    look = v3Normalize(look);
    result.reflected = environment->reflection_function(location, _reflectRay(look, normal)).hit_color;
    refracted = environment->refraction_function(location, _refractRay(look, normal));
    depth = v3Norm(v3Sub(location, refracted.hit_location));
    if (depth > definition->transparency_depth)
    {
        result.refracted = definition->depth_color;
    }
    else
    {
        depth /= definition->transparency_depth;
        result.refracted.r = refracted.hit_color.r * (1.0 - depth) + definition->depth_color.r * depth;
        result.refracted.g = refracted.hit_color.g * (1.0 - depth) + definition->depth_color.g * depth;
        result.refracted.b = refracted.hit_color.b * (1.0 - depth) + definition->depth_color.b * depth;
        result.refracted.a = 1.0;
    }

    color.r = definition->main_color.r * (1.0 - definition->transparency) + result.reflected.r * definition->reflection + result.refracted.r * definition->transparency;
    color.g = definition->main_color.g * (1.0 - definition->transparency) + result.reflected.g * definition->reflection + result.refracted.g * definition->transparency;
    color.b = definition->main_color.b * (1.0 - definition->transparency) + result.reflected.b * definition->reflection + result.refracted.b * definition->transparency;
    color.a = 1.0;

    if (environment->toggle_shadows)
    {
        shadowed = terrainGetShadow(location, sun_direction_inv);
    }
    else
    {
        shadowed = 0.0;
    }
    color = lightingApply(location, normal, shadowed, color, 0.8, 0.6);
    if (environment->toggle_fog)
    {
        color = fogApplyToLocation(location, color);
    }

    result.base = definition->main_color;
    result.final = color;

    return result;
}

Color waterGetColor(Vector3 location, Vector3 look)
{
    return waterGetColorCustom(location, look, &_definition, &_quality, &_environment).final;
}

static int _postProcessFragment(RenderFragment* fragment)
{
    fragment->vertex.color = waterGetColor(fragment->vertex.location, v3Sub(fragment->vertex.location, camera_location));
    return 1;
}

static Vertex _getFirstPassVertex(double x, double z, double precision)
{
    Vertex result;
    double value;

    result.location.x = x;
    result.location.y = _getHeight(&_definition, x, z, 0.0);
    result.location.z = z;
    value = sin(x) * sin(x) * cos(z) * cos(z);
    result.color.r = 0.0;
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

double waterGetLightFactor(Vector3 location)
{
    double factor;

    if (location.y < _definition.height)
    {
        if (sun_direction_inv.y > 0.00001)
        {
            factor = (_definition.height - location.y) / (sun_direction_inv.y * 3.0);
            if (factor > 1.0)
            {
                factor = 1.0;
            }
            return 1.0 - 0.8 * factor;
        }
        else
        {
            return 0.0;
        }
    }
    else
    {
        return 1.0;
    }
}

void waterRender(RenderProgressCallback callback)
{
    int chunk_factor, chunk_count, i;
    double cx = camera_location.x;
    double cz = camera_location.z;
    double radius_int, radius_ext, base_chunk_size, chunk_size;

    base_chunk_size = 2.0 / (double)render_quality;

    chunk_factor = 1;
    chunk_count = 2;
    radius_int = 0.0;
    radius_ext = base_chunk_size;
    chunk_size = base_chunk_size;

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

        if (radius_int > 20.0 && chunk_count % 64 == 0 && (double)chunk_factor < radius_int / 20.0)
        {
            chunk_count /= 2;
            chunk_factor *= 2;
        }
        chunk_count += 2;
        chunk_size = base_chunk_size * chunk_factor;
        radius_int = radius_ext;
        radius_ext += chunk_size;
    }
}

