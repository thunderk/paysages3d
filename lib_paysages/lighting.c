#include <stdlib.h>
#include <math.h>

#include "shared/types.h"
#include "shared/functions.h"
#include "shared/constants.h"
#include "shared/globals.h"

static Color sun_color;
double sun_color_lum;
Vector3 sun_direction;
Vector3 sun_direction_inv;

void lightingSave(FILE* f)
{
}

void lightingLoad(FILE* f)
{
}

void lightingSetSunDirection(double x, double y, double z)
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
}
