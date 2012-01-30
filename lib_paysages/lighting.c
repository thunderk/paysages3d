#include "lighting.h"

#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>

#include "shared/types.h"
#include "shared/constants.h"
#include "color.h"
#include "euclid.h"
#include "renderer.h"
#include "scenery.h"
#include "sky.h"
#include "terrain.h"
#include "tools.h"
#include "water.h"

static LightDefinition _LIGHT_NULL;

void lightingInit()
{
    _LIGHT_NULL.direction.x = 0.0;
    _LIGHT_NULL.direction.y = 1.0;
    _LIGHT_NULL.direction.z = 0.0;
    _LIGHT_NULL.color = COLOR_BLACK;
    _LIGHT_NULL.reflection = 0.0;
    _LIGHT_NULL.filtered = 0;
    _LIGHT_NULL.masked = 0;
    _LIGHT_NULL.amplitude = 0.0;
}

void lightingSave(FILE* f, LightingDefinition* definition)
{
    int i;

    toolsSaveInt(f, &definition->autosetfromsky);
    toolsSaveInt(f, &definition->nblights);
    for (i = 0; i < definition->nblights; i++)
    {
        v3Save(f, &definition->lights[i].direction);
        colorSave(f, &definition->lights[i].color);
        toolsSaveDouble(f, &definition->lights[i].reflection);
        toolsSaveInt(f, &definition->lights[i].filtered);
        toolsSaveInt(f, &definition->lights[i].masked);
        toolsSaveDouble(f, &definition->lights[i].amplitude);
    }
}

void lightingLoad(FILE* f, LightingDefinition* definition)
{
    int i;

    toolsLoadInt(f, &definition->autosetfromsky);
    toolsLoadInt(f, &definition->nblights);
    for (i = 0; i < definition->nblights; i++)
    {
        v3Load(f, &definition->lights[i].direction);
        colorLoad(f, &definition->lights[i].color);
        toolsLoadDouble(f, &definition->lights[i].reflection);
        toolsLoadInt(f, &definition->lights[i].filtered);
        toolsLoadInt(f, &definition->lights[i].masked);
        toolsLoadDouble(f, &definition->lights[i].amplitude);
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
    double diffuse, specular, normal_norm;
    Vector3 view, reflect, direction_inv;

    light = definition->color;

    direction_inv = v3Scale(definition->direction, -1.0);
    if (definition->masked)
    {
        light = renderer->maskLight(renderer, light, location, v3Add(location, v3Scale(direction_inv, 1000.0)), direction_inv);
    }
    if (definition->filtered)
    {
        light = renderer->filterLight(renderer, light, location, v3Add(location, v3Scale(direction_inv, 1000.0)), direction_inv);
    }

    normal_norm = v3Norm(normal);
    if (normal_norm > 1.0)
    {
        normal_norm = 1.0;
    }
    normal = v3Normalize(normal);

    diffuse = v3Dot(direction_inv, normal);
    //diffuse = pow(diffuse * 0.5 + 0.5, 2.0);
    diffuse = diffuse * 0.5 + 0.5;
    if (diffuse > 0.0)
    {
        if (material.shininess > 0.0 && definition->reflection > 0.0)
        {
            view = v3Normalize(v3Sub(location, renderer->camera_location));
            reflect = v3Sub(direction_inv, v3Scale(normal, 2.0 * v3Dot(direction_inv, normal)));

            specular = v3Dot(reflect, view);
            if (specular > 0.0)
            {
                specular = pow(specular, material.shininess);
            }
            else
            {
                specular = 0.0;
            }
            specular *= material.reflection;
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

    specular *= normal_norm * definition->reflection;
    diffuse = 1.0 - normal_norm + diffuse * normal_norm;

    result.r = material.base.r * diffuse * light.r + specular * light.r;
    result.g = material.base.g * diffuse * light.g + specular * light.g;
    result.b = material.base.b * diffuse * light.b + specular * light.b;
    result.a = material.base.a;

    return result;
}

Color lightingApplyToSurface(LightingDefinition* definition, Renderer* renderer, Vector3 location, Vector3 normal, SurfaceMaterial material)
{
    Color result, lighted;
    int i;

    result = COLOR_BLACK;
    result.a = material.base.a;

    for (i = 0; i < definition->nblights; i++)
    {
        lighted = _applyLightCustom(definition->lights + i, renderer, location, normal, material);
        result.r += lighted.r;
        result.g += lighted.g;
        result.b += lighted.b;
    }
    for (i = 0; i < definition->_nbautolights; i++)
    {
        lighted = _applyLightCustom(definition->_autolights + i, renderer, location, normal, material);
        result.r += lighted.r;
        result.g += lighted.g;
        result.b += lighted.b;
    }

    return result;
}
