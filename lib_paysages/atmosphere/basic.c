#include "private.h"

#include <assert.h>
#include <math.h>
#include "../renderer.h"

Color basicApplyAerialPerspective(Renderer* renderer, Vector3 location, Color base)
{
    Vector3 direction = v3Sub(location, renderer->camera_location);
    double distance = v3Norm(direction);
    AtmosphereDefinition* definition = renderer->atmosphere->definition;
    double near = 10.0 - definition->humidity * 10.0;
    double far = 200.0 - definition->humidity * 180.0;
    double max = 0.75 + definition->humidity * 0.22;

    assert(near < far);

    if (distance < near)
    {
        return base;
    }
    else
    {
        if (distance > far)
        {
            distance = far;
        }
        double factor = (distance - near) / (far - near);

        /* TODO Get sky color without celestial objects (sun, stars...) */
        double angle = (1.0 - factor) * (1.0 - factor) * (1.0 - factor) * (1.0 - factor) * M_PI_4;
        direction = v3Normalize(direction);
        direction.y = sin(angle);
        Color sky = renderer->atmosphere->getSkyColor(renderer, v3Normalize(direction));

        sky.a = max * factor;
        colorMask(&base, &sky);

        return base;
    }
}

void basicGetLightingStatus(Renderer* renderer, LightStatus* status, Vector3 normal, int opaque)
{
    /* Direct light from the sun */
    LightDefinition light;

    light.direction = v3Scale(renderer->atmosphere->getSunDirection(renderer), -1.0);
    light.color = renderer->atmosphere->definition->sun_color;
    /*light.color.r *= 100.0;
    light.color.g *= 100.0;
    light.color.b *= 100.0;*/
    light.reflection = 1.0;
    light.altered = 1;
    lightingPushLight(status, &light);

    /* TODO Sample other directions */
}
