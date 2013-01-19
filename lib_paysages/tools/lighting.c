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

    return result;
}

void lightingManagerDelete(LightingManager* filter)
{
    free(filter);
}

void lightingManagerRegisterFilter(LightingManager* filter, FuncLightingAlterLight callback, void* data)
{
    if (filter->callbacks_count < MAX_CALLBACK_COUNT)
    {
        filter->callbacks[filter->callbacks_count].filter = callback;
        filter->callbacks[filter->callbacks_count].data = data;
        filter->callbacks_count++;
    }
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

        for (i = 0; i < manager->callbacks_count; i++)
        {
            LightFilterCallback callback = manager->callbacks[i];
            LightDefinition temp = final;
            if (callback.filter(&temp, status->location, callback.data))
            {
                final = temp;
            }
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

    return result;
}

Color lightingApplyOneLight(LightDefinition* light, Vector3 eye, Vector3 location, Vector3 normal, SurfaceMaterial* material)
{
    Color result, light_color;
    double diffuse, specular, normal_norm;
    Vector3 view, reflect, direction_inv;

    light_color = light->color;
    direction_inv = v3Scale(light->direction, -1.0);

    normal_norm = v3Norm(normal);
    if (normal_norm > 1.0)
    {
        normal_norm = 1.0;
    }
    normal = v3Normalize(normal);

    diffuse = v3Dot(direction_inv, normal);
    if (diffuse > 0.0)
    {
        if (material->shininess > 0.0 && light->reflection > 0.0)
        {
            view = v3Normalize(v3Sub(location, eye));
            reflect = v3Sub(direction_inv, v3Scale(normal, 2.0 * v3Dot(direction_inv, normal)));

            specular = v3Dot(reflect, view);
            if (specular > 0.0)
            {
                specular = pow(specular, material->shininess) * material->reflection;
            }
            else
            {
                specular = 0.0;
            }
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

    specular *= normal_norm * light->reflection;
    diffuse = 1.0 - normal_norm + diffuse * normal_norm;

    result.r = material->base.r * diffuse * light_color.r + specular * light_color.r;
    result.g = material->base.g * diffuse * light_color.g + specular * light_color.g;
    result.b = material->base.b * diffuse * light_color.b + specular * light_color.b;
    result.a = material->base.a;

    return result;
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
