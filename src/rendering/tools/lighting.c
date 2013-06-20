#include "lighting.h"

#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>

#define MAX_CALLBACK_COUNT 10
#define MAX_LIGHT_COUNT 30

typedef struct
{
    FuncLightingAlterLight filter;
    void* data;
} LightFilterCallback;

struct LightingManager
{
    int specularity_enabled;
    int callbacks_count;
    LightFilterCallback callbacks[MAX_CALLBACK_COUNT];
};

struct LightStatus
{
    LightingManager* manager;
    Vector3 location;
    Vector3 eye;
    int light_count;
    LightDefinition lights[MAX_LIGHT_COUNT];
};

LightingManager* lightingManagerCreate()
{
    LightingManager* result;

    result = malloc(sizeof(LightingManager));
    result->callbacks_count = 0;
    result->specularity_enabled = 1;

    return result;
}

void lightingManagerDelete(LightingManager* filter)
{
    free(filter);
}

void lightingManagerRegisterFilter(LightingManager* filter, FuncLightingAlterLight callback, void* data)
{
    int i;

    for (i = 0; i < filter->callbacks_count; i++)
    {
        if (filter->callbacks[i].filter == callback)
        {
            filter->callbacks[i].data = data;
            return;
        }
    }

    if (filter->callbacks_count < MAX_CALLBACK_COUNT)
    {
        filter->callbacks[filter->callbacks_count].filter = callback;
        filter->callbacks[filter->callbacks_count].data = data;
        filter->callbacks_count++;
    }
}

void lightingManagerDisableSpecularity(LightingManager* manager)
{
    manager->specularity_enabled = 0;
}

LightStatus* lightingCreateStatus(LightingManager* manager, Vector3 location, Vector3 eye)
{
    LightStatus* result;

    result = malloc(sizeof(LightStatus));
    result->manager = manager;
    result->location = location;
    result->eye = eye;
    result->light_count = 0;

    return result;
}

void lightingDeleteStatus(LightStatus* status)
{
    free(status);
}

void lightingPushLight(LightStatus* status, LightDefinition* light)
{
    if (status->light_count < MAX_LIGHT_COUNT)
    {
        int i;
        LightingManager* manager = status->manager;
        LightDefinition final = *light;

        if (light->altered)
        {
            for (i = 0; i < manager->callbacks_count; i++)
            {
                LightFilterCallback callback = manager->callbacks[i];
                LightDefinition temp = final;
                if (callback.filter(callback.data, &temp, status->location))
                {
                    final = temp;
                }
            }
        }

        if (!status->manager->specularity_enabled)
        {
            final.reflection = 0.0;
        }

        status->lights[status->light_count++] = final;
    }
}

Color lightingApplyStatus(LightStatus* status, Vector3 normal, SurfaceMaterial* material)
{
    int i;
    Color final, result;

    final.r = final.g = final.b = 0.0;
    final.a = material->base.a;

    for (i = 0; i < status->light_count; i++)
    {
        result = lightingApplyOneLight(status->lights + i, status->eye, status->location, normal, material);
        final.r += result.r;
        final.g += result.g;
        final.b += result.b;
    }

    return final;
}

Color lightingApplyOneLight(LightDefinition* light, Vector3 eye, Vector3 location, Vector3 normal, SurfaceMaterial* material)
{
    Color result, light_color;
    double normal_norm;
    Vector3 direction_inv;

    light_color = light->color;
    direction_inv = v3Scale(v3Normalize(light->direction), -1.0);

    normal_norm = v3Norm(normal);
    if (normal_norm > 1.0)
    {
        normal_norm = 1.0;
    }
    normal = v3Normalize(normal);

    result = COLOR_BLACK;

    /* diffused light */
    double diffuse = v3Dot(direction_inv, normal);
    diffuse = (diffuse + (1.0 - normal_norm)) / (1.0 + (1.0 - normal_norm));
    if (diffuse > 0.0)
    {
        result.r += diffuse * material->base.r * light_color.r;
        result.g += diffuse * material->base.g * light_color.g;
        result.b += diffuse * material->base.b * light_color.b;
    }

    /* specular reflection */
    if (material->reflection > 0.0 && light->reflection > 0.0)
    {
        Vector3 view = v3Normalize(v3Sub(location, eye));
        Vector3 reflect = v3Sub(direction_inv, v3Scale(normal, 2.0 * v3Dot(direction_inv, normal)));
        double specular = v3Dot(reflect, view);
        if (specular > 0.0)
        {
            specular = pow(specular, material->shininess) * material->reflection * light->reflection * normal_norm;
            if (specular > 0.0)
            {
                result.r += specular * light_color.r;
                result.g += specular * light_color.g;
                result.b += specular * light_color.b;
            }
        }
    }

    /* specular reflection with fresnel effect */
    /*if (material->reflection > 0.0 && light->reflection > 0.0)
    {
        Vector3 view = v3Normalize(v3Sub(location, eye));
        Vector3 h = v3Normalize(v3Sub(direction_inv, view));
        double fresnel = 0.02 + 0.98 * pow(1.0 - v3Dot(v3Scale(view, -1.0), h), 5.0);
        double refl = v3Dot(h, normal);
        if (refl > 0.0)
        {
            double waterBrdf = fresnel * pow(refl, material->shininess);
            if (waterBrdf > 0.0)
            {
                refl = material->reflection * waterBrdf * light->reflection;
                result.r += refl * light_color.r;
                result.g += refl * light_color.g;
                result.b += refl * light_color.b;
            }
        }
    }*/

    return result;
}

Vector3 lightingGetStatusLocation(LightStatus* status)
{
    return status->location;
}

void materialSave(PackStream* stream, SurfaceMaterial* material)
{
    colorSave(stream, &material->base);
    packWriteDouble(stream, &material->reflection);
    packWriteDouble(stream, &material->shininess);
}

void materialLoad(PackStream* stream, SurfaceMaterial* material)
{
    colorLoad(stream, &material->base);
    packReadDouble(stream, &material->reflection);
    packReadDouble(stream, &material->shininess);
}
