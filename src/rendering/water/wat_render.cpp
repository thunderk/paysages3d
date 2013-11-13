#include "private.h"

#include <cmath>
#include "../tools.h"
#include "../renderer.h"
#include "WaterDefinition.h"
#include "SurfaceMaterial.h"
#include "NoiseGenerator.h"
#include "terrain/public.h"

static HeightInfo _FAKE_HEIGHT_INFO = {0.0, 0.0, 0.0};

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

    UNUSED(renderer);

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
static inline double _getHeight(WaterDefinition* definition, double base_height, double x, double z)
{
    return base_height + definition->_waves_noise->get2DTotal(x, z);
}

static inline Vector3 _getNormal(WaterDefinition* definition, double base_height, Vector3 base, double detail)
{
    Vector3 back, right;
    double x, z;

    x = base.x;
    z = base.z;

    back.x = x;
    back.y = _getHeight(definition, base_height, x, z + detail);
    back.z = z + detail;
    back = v3Sub(back, base);

    right.x = x + detail;
    right.y = _getHeight(definition, base_height, x + detail, z);
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

static inline Color _getFoamMask(Renderer* renderer, WaterDefinition* definition, Vector3 location, Vector3 normal, double detail)
{
    Color result;
    double foam_factor, normal_diff, location_offset;
    double base_height;

    base_height = renderer->terrain->getWaterHeight(renderer);
    location_offset = 2.0 * detail;

    foam_factor = 0.0;
    location.x += location_offset;
    normal_diff = 1.0 - v3Dot(normal, _getNormal(definition, base_height, location, detail));
    if (normal_diff > foam_factor)
    {
        foam_factor = normal_diff;
    }
    location.x -= location_offset * 2.0;
    normal_diff = 1.0 - v3Dot(normal, _getNormal(definition, base_height, location, detail));
    if (normal_diff > foam_factor)
    {
        foam_factor = normal_diff;
    }
    location.x += location_offset;
    location.z -= location_offset;
    normal_diff = 1.0 - v3Dot(normal, _getNormal(definition, base_height, location, detail));
    if (normal_diff > foam_factor)
    {
        foam_factor = normal_diff;
    }
    location.z += location_offset * 2.0;
    normal_diff = 1.0 - v3Dot(normal, _getNormal(definition, base_height, location, detail));
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
        return COLOR_TRANSPARENT;
    }
    foam_factor = (foam_factor - (1.0 - definition->foam_coverage)) * definition->foam_coverage;

    /* TODO Re-use base lighting status */
    result = renderer->applyLightingToSurface(renderer, location, normal, definition->foam_material);
    result.r *= 2.0;
    result.g *= 2.0;
    result.b *= 2.0;

    /* TODO This should be configurable */
    if (foam_factor > 0.2)
    {
        result.a = 0.8;
    }
    else
    {
        result.a = 0.8 * (foam_factor / 0.2);
    }

    return result;
}

static int _alterLight(Renderer* renderer, LightDefinition* light, Vector3 at)
{
    WaterDefinition* definition = renderer->water->definition;
    double factor;
    double base_height;

    base_height = renderer->terrain->getWaterHeight(renderer);
    if (at.y < base_height)
    {
        if (light->direction.y <= -0.00001)
        {
            factor = (base_height - at.y) / (-light->direction.y * definition->lighting_depth);
            if (factor > 1.0)
            {
                factor = 1.0;
            }
            factor = 1.0 - factor;

            light->color.r *= factor;
            light->color.g *= factor;
            light->color.b *= factor;
            light->reflection *= factor;

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

    info.base_height = renderer->terrain->getWaterHeight(renderer);
    definition->_waves_noise->getRange(&noise_minvalue, &noise_maxvalue);
    info.min_height = info.base_height + noise_minvalue;
    info.max_height = info.base_height + noise_maxvalue;

    return info;
}

static double _realGetHeight(Renderer* renderer, double x, double z)
{
    return _getHeight(renderer->water->definition, renderer->terrain->getWaterHeight(renderer), x, z);
}

static WaterResult _realGetResult(Renderer* renderer, double x, double z)
{
    WaterDefinition* definition = renderer->water->definition;
    WaterResult result;
    RayCastingResult refracted;
    Vector3 location, normal, look_direction;
    Color color, foam;
    double base_height, detail, depth;

    base_height = renderer->terrain->getWaterHeight(renderer);

    location.x = x;
    location.y = _getHeight(definition, base_height, x, z);
    location.z = z;
    result.location = location;

    detail = renderer->getPrecision(renderer, location) * 0.1;
    if (detail < 0.00001)
    {
        detail = 0.00001;
    }

    normal = _getNormal(definition, base_height, location, detail);
    look_direction = v3Normalize(v3Sub(location, renderer->getCameraLocation(renderer, location)));

    /* Reflection */
    if (definition->reflection == 0.0)
    {
        result.reflected = COLOR_BLACK;
    }
    else
    {
        result.reflected = renderer->rayWalking(renderer, location, _reflectRay(look_direction, normal), 1, 0, 1, 1).hit_color;
    }

    /* Transparency/refraction */
    if (definition->transparency == 0.0)
    {
        result.refracted = COLOR_BLACK;
    }
    else
    {
        Color depth_color = *definition->depth_color;
        refracted = renderer->rayWalking(renderer, location, _refractRay(look_direction, normal), 1, 0, 1, 1);
        depth = v3Norm(v3Sub(location, refracted.hit_location));
        colorLimitPower(&depth_color, colorGetPower(&refracted.hit_color));
        if (depth > definition->transparency_depth)
        {
            result.refracted = depth_color;
        }
        else
        {
            depth /= definition->transparency_depth;
            result.refracted.r = refracted.hit_color.r * (1.0 - depth) + depth_color.r * depth;
            result.refracted.g = refracted.hit_color.g * (1.0 - depth) + depth_color.g * depth;
            result.refracted.b = refracted.hit_color.b * (1.0 - depth) + depth_color.b * depth;
            result.refracted.a = 1.0;
        }
    }

    /* Lighting from environment */
    color = renderer->applyLightingToSurface(renderer, location, normal, definition->material);

    color.r += result.reflected.r * definition->reflection + result.refracted.r * definition->transparency;
    color.g += result.reflected.g * definition->reflection + result.refracted.g * definition->transparency;
    color.b += result.reflected.b * definition->reflection + result.refracted.b * definition->transparency;

    /* Merge with foam */
    foam = _getFoamMask(renderer, definition, location, normal, detail);
    colorMask(&color, &foam);

    /* Bring color to the camera */
    color = renderer->applyMediumTraversal(location, color);

    result.base = definition->material->_rgb;
    result.final = color;

    return result;
}

/******************** Renderer ********************/
static WaterRenderer* _createRenderer()
{
    WaterRenderer* result;

    result = new WaterRenderer;
    result->definition = new WaterDefinition(NULL);

    result->getHeightInfo = _fakeGetHeightInfo;
    result->getHeight = _fakeGetHeight;
    result->getResult = _fakeGetResult;

    return result;
}

static void _deleteRenderer(WaterRenderer* renderer)
{
    delete renderer->definition;
    delete renderer;
}

static void _bindRenderer(Renderer* renderer, WaterDefinition* definition)
{
    definition->copy(renderer->water->definition);

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
