#include "atmosphere.h"

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

void atmosphereSave(PackStream* stream, AtmosphereDefinition* definition)
{
    packWriteFloat(stream, &definition->distance_near);
    packWriteFloat(stream, &definition->distance_far);
    packWriteFloat(stream, &definition->full_mask);
    packWriteInt(stream, &definition->auto_lock_on_haze);
    colorSave(stream, &definition->color);
}

void atmosphereLoad(PackStream* stream, AtmosphereDefinition* definition)
{
    packReadFloat(stream, &definition->distance_near);
    packReadFloat(stream, &definition->distance_far);
    packReadFloat(stream, &definition->full_mask);
    packReadInt(stream, &definition->auto_lock_on_haze);
    colorLoad(stream, &definition->color);

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
        definition->color = colorGradationGet(sky.haze_color, sky.daytime);
        skyDeleteDefinition(&sky);
    }
}

Color atmosphereApply(AtmosphereDefinition* definition, Renderer* renderer, Vector3 location, Color base)
{
    Color mask = definition->color;
    float distance = v3Norm(v3Sub(renderer->camera_location, location));
    float value;

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
