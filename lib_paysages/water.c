#include "water.h"

#include "shared/types.h"
#include "render.h"
#include "tools.h"

#include <math.h>

void waterSave(PackStream* stream, WaterDefinition* definition)
{
    packWriteDouble(stream, &definition->height);
    materialSave(stream, &definition->material);
    colorSave(stream, &definition->depth_color);
    packWriteDouble(stream, &definition->transparency_depth);
    packWriteDouble(stream, &definition->transparency);
    packWriteDouble(stream, &definition->reflection);
    packWriteDouble(stream, &definition->lighting_depth);

    packWriteDouble(stream, &definition->scaling);
    packWriteDouble(stream, &definition->waves_height);
    packWriteDouble(stream, &definition->detail_height);
    packWriteDouble(stream, &definition->turbulence);

    packWriteDouble(stream, &definition->foam_coverage);
    materialSave(stream, &definition->foam_material);

    noiseSaveGenerator(stream, definition->_waves_noise);
}

void waterLoad(PackStream* stream, WaterDefinition* definition)
{
    packReadDouble(stream, &definition->height);
    materialLoad(stream, &definition->material);
    colorLoad(stream, &definition->depth_color);
    packReadDouble(stream, &definition->transparency_depth);
    packReadDouble(stream, &definition->transparency);
    packReadDouble(stream, &definition->reflection);
    packReadDouble(stream, &definition->lighting_depth);

    packReadDouble(stream, &definition->scaling);
    packReadDouble(stream, &definition->waves_height);
    packReadDouble(stream, &definition->detail_height);
    packReadDouble(stream, &definition->turbulence);

    packReadDouble(stream, &definition->foam_coverage);
    materialLoad(stream, &definition->foam_material);

    noiseLoadGenerator(stream, definition->_waves_noise);

    waterValidateDefinition(definition);
}

WaterDefinition waterCreateDefinition()
{
    WaterDefinition result;

    result.height = -4.0;
    result._waves_noise = noiseCreateGenerator();

    waterAutoPreset(&result, WATER_PRESET_LAKE);

    return result;
}

void waterDeleteDefinition(WaterDefinition* definition)
{
    noiseDeleteGenerator(definition->_waves_noise);
}

void waterAutoPreset(WaterDefinition* definition, WaterPreset preset)
{
    if (preset == WATER_PRESET_LAKE)
    {
        definition->transparency = 0.5;
        definition->reflection = 0.4;
        definition->transparency_depth = 4.0;
        definition->material.base.r = 0.08;
        definition->material.base.g = 0.15;
        definition->material.base.b = 0.2;
        definition->depth_color.r = 0.0;
        definition->depth_color.g = 0.1;
        definition->depth_color.b = 0.1;
        definition->lighting_depth = 6.0;
        definition->scaling = 1.0;
        definition->waves_height = 0.8;
        definition->detail_height = 0.05;
        definition->turbulence = 0.1;
        definition->foam_coverage = 0.15;
    }
    else if (preset == WATER_PRESET_SEA)
    {
        definition->transparency = 0.4;
        definition->reflection = 0.35;
        definition->transparency_depth = 3.0;
        definition->material.base.r = 0.05;
        definition->material.base.g = 0.18;
        definition->material.base.b = 0.2;
        definition->depth_color.r = 0.0;
        definition->depth_color.g = 0.18;
        definition->depth_color.b = 0.15;
        definition->lighting_depth = 4.0;
        definition->scaling = 1.5;
        definition->waves_height = 1.0;
        definition->detail_height = 0.06;
        definition->turbulence = 0.3;
        definition->foam_coverage = 0.4;
    }

    definition->depth_color.a = 1.0;
    definition->material.base.a = 1.0;
    definition->material.reflection = 1.0;
    definition->material.shininess = 16.0;
    definition->foam_material.base.r = 0.8;
    definition->foam_material.base.g = 0.8;
    definition->foam_material.base.b = 0.8;
    definition->foam_material.base.a = 1.0;
    definition->foam_material.reflection = 0.4;
    definition->foam_material.shininess = 1.5;

    waterValidateDefinition(definition);
}

void waterCopyDefinition(WaterDefinition* source, WaterDefinition* destination)
{
    NoiseGenerator* noise;

    noise = destination->_waves_noise;
    *destination = *source;
    destination->_waves_noise = noise;
    noiseCopy(source->_waves_noise, destination->_waves_noise);
}

void waterValidateDefinition(WaterDefinition* definition)
{
    double scaling = definition->scaling * 0.3;
    noiseClearLevels(definition->_waves_noise);
    if (definition->waves_height > 0.0)
    {
        noiseAddLevelsSimple(definition->_waves_noise, 2, scaling, definition->waves_height * scaling * 0.03);
    }
    if (definition->detail_height > 0.0)
    {
        noiseAddLevelsSimple(definition->_waves_noise, 3, scaling * 0.1, definition->detail_height * scaling * 0.03);
    }
    noiseSetFunctionParams(definition->_waves_noise, NOISE_FUNCTION_SIMPLEX, -definition->turbulence);
    noiseValidate(definition->_waves_noise);
}

static inline double _getHeight(WaterDefinition* definition, double x, double z)
{
    return definition->height + noiseGet2DTotal(definition->_waves_noise, x, z);
}

static inline Vector3 _getNormal(WaterDefinition* definition, Vector3 base, double detail)
{
    Vector3 back, right;
    double x, z;

    x = base.x;
    z = base.z;

    back.x = x;
    back.y = _getHeight(definition, x, z + detail);
    back.z = z + detail;
    back = v3Sub(back, base);

    right.x = x + detail;
    right.y = _getHeight(definition, x + detail, z);
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

HeightInfo waterGetHeightInfo(WaterDefinition* definition)
{
    HeightInfo info;

    info.base_height = definition->height;
    info.min_height = definition->height - noiseGetMaxValue(definition->_waves_noise);
    info.max_height = definition->height + noiseGetMaxValue(definition->_waves_noise);

    return info;
}

Color waterLightFilter(WaterDefinition* definition, Renderer* renderer, Color light, Vector3 location, Vector3 light_location, Vector3 direction_to_light)
{
    double factor;

    UNUSED(renderer);
    UNUSED(light_location);

    if (location.y < definition->height)
    {
        if (direction_to_light.y > 0.00001)
        {
            factor = (definition->height - location.y) / (direction_to_light.y * definition->lighting_depth);
            if (factor > 1.0)
            {
                factor = 1.0;
            }
            factor = 1.0 - 0.8 * factor;

            light.r *= factor;
            light.g *= factor;
            light.b *= factor;

            return light;
        }
        else
        {
            return COLOR_BLACK;
        }
    }
    else
    {
        return light;
    }
}

static inline void _applyFoam(WaterDefinition* definition, Vector3 location, Vector3 normal, double detail, SurfaceMaterial* material)
{
    Color result = definition->foam_material.base;
    double foam_factor, normal_diff, location_offset;

    location_offset = 2.0 * detail;

    foam_factor = 0.0;
    location.x += location_offset;
    normal_diff = 1.0 - v3Dot(normal, _getNormal(definition, location, detail));
    if (normal_diff > foam_factor)
    {
        foam_factor = normal_diff;
    }
    location.x -= location_offset * 2.0;
    normal_diff = 1.0 - v3Dot(normal, _getNormal(definition, location, detail));
    if (normal_diff > foam_factor)
    {
        foam_factor = normal_diff;
    }
    location.x += location_offset;
    location.z -= location_offset;
    normal_diff = 1.0 - v3Dot(normal, _getNormal(definition, location, detail));
    if (normal_diff > foam_factor)
    {
        foam_factor = normal_diff;
    }
    location.z += location_offset * 2.0;
    normal_diff = 1.0 - v3Dot(normal, _getNormal(definition, location, detail));
    if (normal_diff > foam_factor)
    {
        foam_factor = normal_diff;
    }

    foam_factor *= 10.0;
    if (foam_factor > 1.0)
    {
        foam_factor = 1.0;
    }

    if (foam_factor <= 1.0 - definition->foam_coverage)
    {
        return;
    }
    foam_factor = (foam_factor - (1.0 - definition->foam_coverage)) * definition->foam_coverage;

    material->reflection = foam_factor * definition->foam_material.reflection + (1.0 - foam_factor) * material->reflection;
    material->shininess = foam_factor * definition->foam_material.shininess + (1.0 - foam_factor) * material->shininess;

    /* TODO This should be configurable */
    if (foam_factor > 0.2)
    {
        result.a = 0.8;
    }
    else
    {
        result.a = 0.8 * (foam_factor / 0.2);
    }
    colorMask(&material->base, &result);
}

WaterResult waterGetColorDetail(WaterDefinition* definition, Renderer* renderer, Vector3 location, Vector3 look)
{
    WaterResult result;
    RayCastingResult refracted;
    Vector3 normal;
    Color color;
    SurfaceMaterial material;
    double detail, depth;

    detail = renderer->getPrecision(renderer, location) * 0.1;
    if (detail < 0.00001)
    {
        detail = 0.00001;
    }

    location.y = _getHeight(definition, location.x, location.z);
    result.location = location;

    normal = _getNormal(definition, location, detail);
    look = v3Normalize(look);
    result.reflected = renderer->rayWalking(renderer, location, _reflectRay(look, normal), 1, 0, 1, 1).hit_color;
    refracted = renderer->rayWalking(renderer, location, _refractRay(look, normal), 1, 0, 1, 1);
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

    color.r = definition->material.base.r * (1.0 - definition->transparency) + result.reflected.r * definition->reflection + result.refracted.r * definition->transparency;
    color.g = definition->material.base.g * (1.0 - definition->transparency) + result.reflected.g * definition->reflection + result.refracted.g * definition->transparency;
    color.b = definition->material.base.b * (1.0 - definition->transparency) + result.reflected.b * definition->reflection + result.refracted.b * definition->transparency;
    color.a = 1.0;

    material = definition->material;
    material.base = color;

    _applyFoam(definition, location, normal, detail, &material);

    color = renderer->applyLightingToSurface(renderer, location, normal, &material);
    color = renderer->atmosphere->applyAerialPerspective(renderer, location, color);
    color = renderer->applyClouds(renderer, color, renderer->camera_location, location);

    result.base = definition->material.base;
    result.final = color;

    return result;
}

Color waterGetColor(WaterDefinition* definition, Renderer* renderer, Vector3 location, Vector3 look)
{
    return waterGetColorDetail(definition, renderer, location, look).final;
}

static Color _postProcessFragment(Renderer* renderer, Vector3 location, void* data)
{
    return waterGetColor((WaterDefinition*)data, renderer, location, v3Sub(location, renderer->camera_location));
}

static Vector3 _getFirstPassVertex(WaterDefinition* definition, double x, double z, double precision)
{
    Vector3 result;

    result.x = x;
    result.y = _getHeight(definition, x, z);
    result.z = z;

    return result;
}

static void _renderQuad(WaterDefinition* definition, Renderer* renderer, double x, double z, double size)
{
    Vector3 v1, v2, v3, v4;

    v1 = _getFirstPassVertex(definition, x, z, size);
    v2 = _getFirstPassVertex(definition, x, z + size, size);
    v3 = _getFirstPassVertex(definition, x + size, z + size, size);
    v4 = _getFirstPassVertex(definition, x + size, z, size);

    renderer->pushQuad(renderer, v1, v2, v3, v4, _postProcessFragment, definition);
}

void waterRender(WaterDefinition* definition, Renderer* renderer)
{
    int chunk_factor, chunk_count, i;
    double cx = renderer->camera_location.x;
    double cz = renderer->camera_location.z;
    double radius_int, radius_ext, base_chunk_size, chunk_size;

    base_chunk_size = 2.0 / (double)renderer->render_quality;
    if (renderer->render_quality > 7)
    {
        base_chunk_size *= 0.5;
    }

    chunk_factor = 1;
    chunk_count = 2;
    radius_int = 0.0;
    radius_ext = base_chunk_size;
    chunk_size = base_chunk_size;

    while (radius_int < 5000.0)
    {
        if (!renderer->addRenderProgress(renderer, 0.0))
        {
            return;
        }

        for (i = 0; i < chunk_count - 1; i++)
        {
            _renderQuad(definition, renderer, cx - radius_ext + chunk_size * i, cz - radius_ext, chunk_size);
            _renderQuad(definition, renderer, cx + radius_int, cz - radius_ext + chunk_size * i, chunk_size);
            _renderQuad(definition, renderer, cx + radius_int - chunk_size * i, cz + radius_int, chunk_size);
            _renderQuad(definition, renderer, cx - radius_ext, cz + radius_int - chunk_size * i, chunk_size);
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

