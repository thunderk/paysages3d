#include "lighting.h"

#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>

#include "shared/types.h"
#include "shared/functions.h"
#include "shared/constants.h"
#include "shared/globals.h"
#include "sky.h"
#include "water.h"
#include "terrain.h"

static LightingDefinition _definition;
static LightingQuality _quality;
static LightingEnvironment _environment;

static LightDefinition _LIGHT_NULL;

static Color _standardFilter(Color light, Vector3 location, Vector3 light_location, Vector3 direction_to_light, void* custom_data)
{
    Color result;

    result = waterLightFilter(light, location, light_location, direction_to_light, custom_data);
    result = terrainLightFilter(result, location, light_location, direction_to_light, custom_data);
    // TODO atmosphere filter
    // TODO clouds filter

    return result;
}

void lightingInit()
{
    _definition = lightingCreateDefinition();

    _environment.filter = _standardFilter;
    _environment.custom_data = NULL;

    _LIGHT_NULL.color = COLOR_BLACK;
    _LIGHT_NULL.direction.x = 0.0;
    _LIGHT_NULL.direction.y = 1.0;
    _LIGHT_NULL.direction.z = 0.0;
}

void lightingSave(FILE* f)
{
    // TODO
}

void lightingLoad(FILE* f)
{
    // TODO
}

LightingDefinition lightingCreateDefinition()
{
    LightingDefinition definition;

    definition.autosetfromsky = 0;
    definition.nblights = 0;
    definition._nbautolights = 0;

    return definition;
}

void lightingDeleteDefinition(LightingDefinition definition)
{
}

void lightingCopyDefinition(LightingDefinition source, LightingDefinition* destination)
{
    *destination = source;
}

void lightingSetDefinition(LightingDefinition definition)
{
    lightingValidateDefinition(&definition);
    lightingCopyDefinition(definition, &_definition);
}

LightingDefinition lightingGetDefinition()
{
    return _definition;
}

void lightingValidateDefinition(LightingDefinition* definition)
{
    if (!definition)
    {
        lightingValidateDefinition(&_definition);
        return;
    }

    if (definition->autosetfromsky)
    {
        // TODO Get lights from sky
        definition->_nbautolights = skyGetLights(definition->_autolights, MAX_LIGHTS);
    }
    else
    {
        definition->_nbautolights = 0;
    }
}

int lightingGetLightCount(LightingDefinition* definition)
{
    return definition->nblights;
}

LightDefinition lightingGetLight(LightingDefinition* definition, int light)
{
    if (light >= 0 && light < definition->nblights)
    {
        return definition->lights[light];
    }
    else
    {
        return _LIGHT_NULL;
    }
}

int lightingAddLight(LightingDefinition* definition, LightDefinition light)
{
    if (definition->nblights < MAX_LIGHTS)
    {
        definition->lights[definition->nblights] = light;
        return definition->nblights++;
    }
    else
    {
        return -1;
    }
}

void lightingDeleteLight(LightingDefinition* definition, int light)
{
    if (light >= 0 && light < definition->nblights)
    {
        if (definition->nblights > 1 && light < definition->nblights - 1)
        {
            memmove(definition->lights + light, definition->lights + light + 1, sizeof(LightDefinition) * definition->nblights - light - 1);
        }
        definition->nblights--;
    }
}

void lightingSetQuality(LightingQuality quality)
{
    _quality = quality;
}

LightingQuality lightingGetQuality()
{
    return _quality;
}

static Color _applyLightCustom(Vector3 location, Vector3 normal, ReceiverMaterial material, LightDefinition* definition, LightingQuality* quality, LightingEnvironment* environment)
{
    Color result, light;
    double diffuse, specular;
    Vector3 view, reflect, direction_inv;

    light = definition->color;

    direction_inv = v3Scale(definition->direction, -1.0);
    light = environment->filter(light, location, v3Add(location, direction_inv), direction_inv, environment->custom_data);

    normal = v3Normalize(normal);
    view = v3Normalize(v3Sub(location, camera_location)); // TODO Configurable
    reflect = v3Sub(v3Scale(normal, 2.0 * v3Dot(direction_inv, normal)), direction_inv);

    diffuse = v3Dot(direction_inv, normal);
    //diffuse = pow(diffuse * 0.5 + 0.5, 2.0);
    if (diffuse > 0.0)
    {
        if (material.shininess > 0.0)
        {
            specular = pow(v3Dot(reflect, view) * material.reflection, material.shininess * 10.0 + 1.0);
        }
        else
        {
            specular = 0.0;
        }
    }
    else
    {
        diffuse = 0.0;
        specular = 0.0;
    }

    result.r = material.base.r * diffuse * light.r + material.base.r * specular * light.r;
    result.g = material.base.g * diffuse * light.g + material.base.g * specular * light.g;
    result.b = material.base.b * diffuse * light.b + material.base.b * specular * light.b;
    result.a = material.base.a;

    return result;
}

Color lightingApplyCustom(Vector3 location, Vector3 normal, ReceiverMaterial material, LightingDefinition* definition, LightingQuality* quality, LightingEnvironment* environment)
{
    Color result;
    int i;

    if (!definition)
    {
        definition = &_definition;
    }
    if (!quality)
    {
        quality = &_quality;
    }
    if (!environment)
    {
        environment = &_environment;
    }

    /* TODO Merge lights */
    result = material.base;
    for (i = 0; i < definition->nblights; i++)
    {
        result = _applyLightCustom(location, normal, material, definition->lights + i, quality, environment);
    }
    for (i = 0; i < definition->_nbautolights; i++)
    {
        result = _applyLightCustom(location, normal, material, definition->_autolights + i, quality, environment);
    }
    return result;
}

Color lightingApply(Vector3 location, Vector3 normal, ReceiverMaterial material)
{
    return lightingApplyCustom(location, normal, material, &_definition, &_quality, &_environment);
}
