#include "water.h"

#include "shared/types.h"
#include "color.h"
#include "euclid.h"
#include "render.h"
#include "terrain.h"
#include "lighting.h"
#include "tools.h"

#include <math.h>

void waterInit()
{
}

void waterQuit()
{
}

void waterSave(PackStream* stream, WaterDefinition* definition)
{
    packWriteDouble(stream, &definition->height);
    materialSave(stream, &definition->material);
    colorSave(stream, &definition->depth_color);
    packWriteDouble(stream, &definition->transparency_depth);
    packWriteDouble(stream, &definition->transparency);
    packWriteDouble(stream, &definition->reflection);
    packWriteDouble(stream, &definition->lighting_depth);
    noiseSaveGenerator(stream, definition->waves_noise);
    packWriteDouble(stream, &definition->waves_noise_height);
    packWriteDouble(stream, &definition->waves_noise_scale);
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
    noiseLoadGenerator(stream, definition->waves_noise);
    packReadDouble(stream, &definition->waves_noise_height);
    packReadDouble(stream, &definition->waves_noise_scale);

    waterValidateDefinition(definition);
}

WaterDefinition waterCreateDefinition()
{
    WaterDefinition result;

    result.material.base = COLOR_BLACK;
    result.material.reflection = 0.0;
    result.material.shininess = 0.0;
    result.depth_color = COLOR_BLACK;
    result.height = -1000.0;
    result.reflection = 0.0;
    result.transparency = 0.0;
    result.transparency_depth = 0.0;
    result.lighting_depth = 0.0;
    result.waves_noise = noiseCreateGenerator();
    result.waves_noise_height = 0.02;
    result.waves_noise_scale = 0.2;

    return result;
}

void waterDeleteDefinition(WaterDefinition* definition)
{
    noiseDeleteGenerator(definition->waves_noise);
}

void waterCopyDefinition(WaterDefinition* source, WaterDefinition* destination)
{
    NoiseGenerator* noise;

    noise = destination->waves_noise;
    *destination = *source;
    destination->waves_noise = noise;
    noiseCopy(source->waves_noise, destination->waves_noise);
}

void waterValidateDefinition(WaterDefinition* definition)
{
}

static inline double _getHeight(WaterDefinition* definition, double x, double z, double detail)
{
    return definition->height + noiseGet2DDetail(definition->waves_noise, x / definition->waves_noise_scale, z / definition->waves_noise_scale, detail) * definition->waves_noise_height;
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

HeightInfo waterGetHeightInfo(WaterDefinition* definition)
{
    HeightInfo info;
    
    info.base_height = definition->height;
    info.min_height = definition->height - noiseGetMaxValue(definition->waves_noise) * definition->waves_noise_height;
    info.max_height = definition->height + noiseGetMaxValue(definition->waves_noise) * definition->waves_noise_height;
    
    return info;
}

Color waterLightFilter(WaterDefinition* definition, Renderer* renderer, Color light, Vector3 location, Vector3 light_location, Vector3 direction_to_light)
{
    double factor;

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

WaterResult waterGetColorDetail(WaterDefinition* definition, Renderer* renderer, Vector3 location, Vector3 look)
{
    WaterResult result;
    RayCastingResult refracted;
    Vector3 normal;
    Color color;
    SurfaceMaterial material;
    double detail, depth;

    detail = renderer->getPrecision(renderer, location);

    location.y = _getHeight(definition, location.x, location.z, detail);
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
    color = renderer->applyLightingToSurface(renderer, location, normal, material);
    color = renderer->applyAtmosphere(renderer, location, color);
    color = renderer->applyClouds(renderer, color, renderer->camera_location, location);

    result.base = definition->material.base;
    result.final = color;

    return result;
}

Color waterGetColor(WaterDefinition* definition, Renderer* renderer, Vector3 location, Vector3 look)
{
    return waterGetColorDetail(definition, renderer, location, look).final;
}

static int _postProcessFragment(RenderFragment* fragment, Renderer* renderer, void* data)
{
    fragment->vertex.color = waterGetColor((WaterDefinition*)data, renderer, fragment->vertex.location, v3Sub(fragment->vertex.location, renderer->camera_location));
    return 1;
}

static Vertex _getFirstPassVertex(WaterDefinition* definition, double x, double z, double precision)
{
    Vertex result;
    double value;

    result.location.x = x;
    result.location.y = _getHeight(definition, x, z, 0.0);
    result.location.z = z;
    value = sin(x) * sin(x) * cos(z) * cos(z);
    result.color.r = 0.0;
    result.color.g = value;
    result.color.b = value;
    result.color.a = 1.0;
    result.normal.x = result.normal.y = result.normal.z = 0.0;
    result.callback = _postProcessFragment;
    result.callback_data = definition;

    return result;
}

static void _renderQuad(WaterDefinition* definition, Renderer* renderer, double x, double z, double size)
{
    Vertex v1, v2, v3, v4;

    v1 = _getFirstPassVertex(definition, x, z, size);
    v2 = _getFirstPassVertex(definition, x, z + size, size);
    v3 = _getFirstPassVertex(definition, x + size, z + size, size);
    v4 = _getFirstPassVertex(definition, x + size, z, size);
    renderer->pushQuad(renderer, &v1, &v2, &v3, &v4);
}

void waterRender(WaterDefinition* definition, Renderer* renderer)
{
    int chunk_factor, chunk_count, i;
    double cx = renderer->camera_location.x;
    double cz = renderer->camera_location.z;
    double radius_int, radius_ext, base_chunk_size, chunk_size;

    base_chunk_size = 2.0 / (double)renderer->render_quality;

    chunk_factor = 1;
    chunk_count = 2;
    radius_int = 0.0;
    radius_ext = base_chunk_size;
    chunk_size = base_chunk_size;

    while (radius_ext < 1000.0)
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

