#include "private.h"

/*
 * Atmosphere presets.
 */

void atmosphereAutoPreset(AtmosphereDefinition* definition, AtmospherePreset preset)
{
    definition->sun_color.r = 1.0;
    definition->sun_color.g = 0.95;
    definition->sun_color.b = 0.9;
    definition->sun_color.a = 1.0;
    definition->sun_radius = 1.0;
    definition->humidity = 0.1;

    definition->model = ATMOSPHERE_MODEL_BRUNETON;

    switch (preset)
    {
        case ATMOSPHERE_PRESET_CLEAR_DAY:
            definition->hour = 15;
            definition->minute = 0;
            definition->dome_lighting = 0.2;
            break;
        case ATMOSPHERE_PRESET_CLEAR_SUNSET:
            definition->hour = 17;
            definition->minute = 45;
            definition->dome_lighting = 0.3;
            definition->sun_radius = 0.03;
            break;
        case ATMOSPHERE_PRESET_HAZY_MORNING:
            definition->hour = 8;
            definition->minute = 30;
            definition->dome_lighting = 0.25;
            definition->humidity = 0.4;
            break;
        case ATMOSPHERE_PRESET_FOGGY:
            definition->hour = 15;
            definition->minute = 0;
            definition->dome_lighting = 0.1;
            definition->humidity = 0.7;
            break;
        case ATMOSPHERE_PRESET_STORMY:
            definition->hour = 15;
            definition->minute = 0;
            definition->dome_lighting = 0.05;
            definition->humidity = 0.9;
            break;
        default:
            ;
    }

    AtmosphereDefinitionClass.validate(definition);
}
