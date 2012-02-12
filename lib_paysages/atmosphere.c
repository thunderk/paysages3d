#include "atmosphere.h"

#include "shared/constants.h"
#include "scenery.h"
#include "euclid.h"
#include "color.h"
#include "tools.h"

void atmosphereInit()
{
}

void atmosphereQuit()
{
}

void atmosphereSave(FILE* f, AtmosphereDefinition* definition)
{
    toolsSaveDouble(f, &definition->distance_near);
    toolsSaveDouble(f, &definition->distance_far);
    toolsSaveDouble(f, &definition->full_mask);
    toolsSaveInt(f, &definition->auto_lock_on_haze);
    colorSave(f, &definition->color);
}

void atmosphereLoad(FILE* f, AtmosphereDefinition* definition)
{
    toolsLoadDouble(f, &definition->distance_near);
    toolsLoadDouble(f, &definition->distance_far);
    toolsLoadDouble(f, &definition->full_mask);
    toolsLoadInt(f, &definition->auto_lock_on_haze);
    colorLoad(f, &definition->color);

    atmosphereValidateDefinition(definition);
}

AtmosphereDefinition atmosphereCreateDefinition()
{
    AtmosphereDefinition definition;

    definition.distance_near = 0.0;
    definition.distance_far = 1.0;
    definition.full_mask = 0.0;
    definition.auto_lock_on_haze = 0;
    definition.color = COLOR_BLACK;

    atmosphereValidateDefinition(&definition);

    return definition;
}

void atmosphereDeleteDefinition(AtmosphereDefinition* definition)
{
}

void atmosphereCopyDefinition(AtmosphereDefinition* source, AtmosphereDefinition* destination)
{
    *destination = *source;
}

void atmosphereValidateDefinition(AtmosphereDefinition* definition)
{
    SkyDefinition sky;

    if (definition->distance_far <= definition->distance_near)
    {
        definition->distance_far = definition->distance_near + 1.0;
    }
    if (definition->full_mask < 0.0)
    {
        definition->full_mask = 0.0;
    }
    if (definition->full_mask > 1.0)
    {
        definition->full_mask = 1.0;
    }
    if (definition->auto_lock_on_haze)
    {
        sky = skyCreateDefinition();
        sceneryGetSky(&sky);
        definition->color = colorGradationGet(&sky.haze_color, sky.daytime);
        skyDeleteDefinition(&sky);
    }
}

Color atmosphereApply(AtmosphereDefinition* definition, Renderer* renderer, Vector3 location, Color base)
{
    Color mask = definition->color;
    double distance = v3Norm(v3Sub(renderer->camera_location, location));
    double value;

    if (distance < definition->distance_near)
    {
        return base;
    }
    else if (distance > definition->distance_far)
    {
        distance = definition->distance_far;
    }

    value = definition->full_mask * (distance - definition->distance_near) / (definition->distance_far - definition->distance_near);
    mask.a = value;

    colorMask(&base, &mask);

    return base;
}
