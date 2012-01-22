#include "lighting.h"

#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>

#include "shared/types.h"
#include "shared/functions.h"
#include "shared/constants.h"
#include "shared/globals.h"

static LightingDefinition _definition;
static LightingQuality _quality;
static LightingEnvironment _environment;

static LightDefinition _LIGHT_NULL;

static Color _standardFilter(Color light, Vector3 location, Vector3 light_location, Vector3 direction_to_light, void* custom_data)
{
    // TODO Find shadows
    return light;
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
    }

    if (definition->autosetfromsky)
    {
        // TODO Get lights from sky
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

Color lightingApplyCustom(Vector3 location, Vector3 normal, ReceiverMaterial material, LightingDefinition* definition, LightingQuality* quality, LightingEnvironment* environment)
{
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

    return COLOR_RED;
}

Color lightingApply(Vector3 location, Vector3 normal, ReceiverMaterial material)
{
    return lightingApplyCustom(location, normal, material, &_definition, &_quality, &_environment);
}

/*void lightingSetSunDirection(double x, double y, double z)
{
    sun_direction.x = x;
    sun_direction.y = y;
    sun_direction.z = z;
    sun_direction = v3Normalize(sun_direction);
    sun_direction_inv = v3Scale(sun_direction, -1.0);
}

void lightingSetSunAngle(double hor, double ver)
{
    lightingSetSunDirection(cos(hor) * cos(ver), sin(ver), -sin(hor) * cos(ver));
}

void lightingSetSunColor(Color col)
{
    sun_color = col;
    sun_color_lum = colorGetValue(&col);
}

Color lightingApply(Vector3 location, Vector3 normal, double shadowing, Color base, double reflection, double shininess)
{
    Color result, light;
    double ambient, diffuse, specular;
    Vector3 view, reflect;

    light.r = sun_color.r * (1.0 - 0.4 * shadowing);
    light.g = sun_color.g * (1.0 - 0.4 * shadowing);
    light.b = sun_color.b * (1.0 - 0.4 * shadowing);

    normal = v3Normalize(normal);
    view = v3Normalize(v3Sub(location, camera_location));
    reflect = v3Sub(v3Scale(normal, 2.0 * v3Dot(sun_direction_inv, normal)), sun_direction_inv);

    ambient = 0.2;
    diffuse = v3Dot(sun_direction_inv, normal);
    diffuse = pow(diffuse * 0.5 + 0.5, 2.0) * (1.0 - shadowing) + (diffuse * 0.5 + 0.3) * shadowing;
    if (diffuse > 0.0)
    {
        if (shininess > 0.0)
        {
            specular = pow(v3Dot(reflect, view) * reflection, shininess * 10.0 + 1.0);
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

    result.r = base.r * ambient + base.r * diffuse * light.r + base.r * specular * light.r;
    result.g = base.g * ambient + base.g * diffuse * light.g + base.g * specular * light.g;
    result.b = base.b * ambient + base.b * diffuse * light.b + base.b * specular * light.b;
    result.a = base.a;

    return result;
}*/
