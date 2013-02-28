#include "private.h"

#include <stdlib.h>
#include <math.h>
#include "../tools.h"
#include "../renderer.h"

static HeightInfo _FAKE_HEIGHT_INFO = {0.0, 0.0, 0.0};
static WaterResult _FAKE_RESULT = {};

/******************** Fake ********************/
static HeightInfo _fakeGetHeightInfo(Renderer* renderer)
{
    UNUSED(renderer);
    return _FAKE_HEIGHT_INFO;
}

static double _fakeGetHeight(Renderer* renderer, double x, double z)
{
    UNUSED(renderer);
    UNUSED(x);
    UNUSED(z);
    return 0.0;
}

static WaterResult _fakeGetResult(Renderer* renderer, double x, double z)
{
    WaterResult result;

    result.location.x = x;
    result.location.y = 0.0;
    result.location.z = z;

    result.base = COLOR_BLUE;
    result.reflected = COLOR_BLACK;
    result.refracted = COLOR_BLACK;
    result.foam = COLOR_BLACK;
    result.final = COLOR_BLUE;

    return result;
}

/******************** Helpers ********************/
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

static int _alterLight(Renderer* renderer, LightDefinition* light, Vector3 at)
{
    WaterDefinition* definition = renderer->water->definition;
    double factor;

    if (at.y < definition->height)
    {
        if (light->direction.y < 0.00001)
        {
            factor = (definition->height - at.y) / (-light->direction.y * definition->lighting_depth);
            if (factor > 1.0)
            {
                factor = 1.0;
            }
            factor = 1.0 - 0.8 * factor;

            light->color.r *= factor;
            light->color.g *= factor;
            light->color.b *= factor;

            return 1;
        }
        else
        {
            light->color = COLOR_BLACK;
            return 1;
        }
    }
    else
    {
        return 0;
    }
}

/******************** Real ********************/
static HeightInfo _realGetHeightInfo(Renderer* renderer)
{
    WaterDefinition* definition = renderer->water->definition;
    HeightInfo info;
    double noise_minvalue, noise_maxvalue;

    info.base_height = definition->height;
    noiseGetRange(definition->_waves_noise, &noise_minvalue, &noise_maxvalue);
    info.min_height = definition->height + noise_minvalue;
    info.max_height = definition->height + noise_maxvalue;

    return info;
}

static double _realGetHeight(Renderer* renderer, double x, double z)
{
    return _getHeight(renderer->water->definition, x, z);
}

static WaterResult _realGetResult(Renderer* renderer, double x, double z)
{
    WaterDefinition* definition = renderer->water->definition;
    WaterResult result;
    RayCastingResult refracted;
    Vector3 location, normal, look;
    Color color;
    SurfaceMaterial material;
    double detail, depth;

    location.x = x;
    location.y = _getHeight(definition, x, z);
    location.z = z;
    result.location = location;

    detail = renderer->getPrecision(renderer, location) * 0.1;
    if (detail < 0.00001)
    {
        detail = 0.00001;
    }

    normal = _getNormal(definition, location, detail);
    look = v3Normalize(renderer->getCameraDirection(renderer, location));
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
    color = renderer->applyMediumTraversal(renderer, location, color);

    result.base = definition->material.base;
    result.final = color;

    return result;
}

/******************** Renderer ********************/
static WaterRenderer* _createRenderer()
{
    WaterRenderer* result;

    result = malloc(sizeof(WaterRenderer));
    result->definition = WaterDefinitionClass.create();

    result->getHeightInfo = _fakeGetHeightInfo;
    result->getHeight = _fakeGetHeight;
    result->getResult = _fakeGetResult;

    return result;
}

static void _deleteRenderer(WaterRenderer* renderer)
{
    WaterDefinitionClass.destroy(renderer->definition);
    free(renderer);
}

static void _bindRenderer(Renderer* renderer, WaterDefinition* definition)
{
    WaterDefinitionClass.copy(definition, renderer->water->definition);

    renderer->water->getHeightInfo = _realGetHeightInfo;
    renderer->water->getHeight = _realGetHeight;
    renderer->water->getResult = _realGetResult;

    lightingManagerRegisterFilter(renderer->lighting, (FuncLightingAlterLight)_alterLight, renderer);
}

StandardRenderer WaterRendererClass = {
    (FuncObjectCreate)_createRenderer,
    (FuncObjectDelete)_deleteRenderer,
    (FuncObjectBind)_bindRenderer
};
