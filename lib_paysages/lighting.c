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
#include "renderer.h"
#include "scenery.h"

static LightDefinition _LIGHT_NULL;

void lightingInit()
{
    _LIGHT_NULL.color = COLOR_BLACK;
    _LIGHT_NULL.direction.x = 0.0;
    _LIGHT_NULL.direction.y = 1.0;
    _LIGHT_NULL.direction.z = 0.0;
}

void lightingSave(FILE* f, LightingDefinition* definition)
{
    int i;

    toolsSaveInt(f, definition->autosetfromsky);
    toolsSaveInt(f, definition->nblights);
    for (i = 0; i < definition->nblights; i++)
    {
        colorSave(definition->lights[i].color, f);
        v3Save(definition->lights[i].direction, f);
        toolsSaveDouble(f, definition->lights[i].maxshadow);
    }
}

void lightingLoad(FILE* f, LightingDefinition* definition)
{
    int i;

    definition->autosetfromsky = toolsLoadInt(f);
    definition->nblights = toolsLoadInt(f);
    for (i = 0; i < definition->nblights; i++)
    {
        definition->lights[i].color = colorLoad(f);
        definition->lights[i].direction = v3Load(f);
        definition->lights[i].maxshadow = toolsLoadDouble(f);
    }

    lightingValidateDefinition(definition);
}

LightingDefinition lightingCreateDefinition()
{
    LightingDefinition definition;

    definition.autosetfromsky = 0;
    definition.nblights = 0;
    definition._nbautolights = 0;

    return definition;
}

void lightingDeleteDefinition(LightingDefinition* definition)
{
}

void lightingCopyDefinition(LightingDefinition* source, LightingDefinition* destination)
{
    *destination = *source;
}

void lightingValidateDefinition(LightingDefinition* definition)
{
    if (definition->autosetfromsky)
    {
        SkyDefinition sky;

        sky = skyCreateDefinition();
        sceneryGetSky(&sky);
        definition->_nbautolights = skyGetLights(&sky, definition->_autolights, LIGHTING_MAX_LIGHTS);
        skyDeleteDefinition(&sky);
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
    if (definition->nblights < LIGHTING_MAX_LIGHTS)
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

static Color _applyLightCustom(LightDefinition* definition, Renderer* renderer, Vector3 location, Vector3 normal, SurfaceMaterial material)
{
    Color result, light;
    double diffuse, specular;
    Vector3 view, reflect, direction_inv;

    light = definition->color;

    direction_inv = v3Scale(definition->direction, -1.0);
    light = renderer->filterLight(renderer, light, location, v3Add(location, direction_inv), direction_inv);

    normal = v3Normalize(normal);
    view = v3Normalize(v3Sub(location, renderer->camera_location));
    reflect = v3Sub(v3Scale(normal, 2.0 * v3Dot(direction_inv, normal)), direction_inv);

    diffuse = v3Dot(direction_inv, normal);
    //diffuse = pow(diffuse * 0.5 + 0.5, 2.0);
    diffuse = diffuse * 0.5 + 0.5;
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

Color lightingApplyToSurface(LightingDefinition* definition, Renderer* renderer, Vector3 location, Vector3 normal, SurfaceMaterial material)
{
    Color result;
    int i;

    /* TODO Merge lights */
    result = material.base;
    for (i = 0; i < definition->nblights; i++)
    {
        result = _applyLightCustom(definition->lights + i, renderer, location, normal, material);
    }
    for (i = 0; i < definition->_nbautolights; i++)
    {
        result = _applyLightCustom(definition->_autolights + i, renderer, location, normal, material);
    }
    return result;
}
