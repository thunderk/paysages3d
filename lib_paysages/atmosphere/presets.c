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
    definition->sun_radius = 0.02;
    definition->humidity = 0.1;

    switch (preset)
    {
        case ATMOSPHERE_PRESET_CLEAR_DAY:
            definition->model = ATMOSPHERE_MODEL_BRUNETON;
            definition->hour = 15;
            definition->minute = 0;
            definition->dome_lighting = 0.2;
            break;
        case ATMOSPHERE_PRESET_CLEAR_SUNSET:
            definition->model = ATMOSPHERE_MODEL_BRUNETON;
            definition->hour = 17;
            definition->minute = 45;
            definition->dome_lighting = 0.3;
            definition->sun_radius = 0.03;
            break;
        case ATMOSPHERE_PRESET_HAZY_MORNING:
            definition->model = ATMOSPHERE_MODEL_PREETHAM;
            definition->hour = 8;
            definition->minute = 30;
            definition->dome_lighting = 0.25;
            definition->humidity = 0.3;
            break;
        case ATMOSPHERE_PRESET_FOGGY:
            definition->model = ATMOSPHERE_MODEL_PREETHAM;
            definition->hour = 15;
            definition->minute = 0;
            definition->dome_lighting = 0.1;
            definition->humidity = 0.6;
            break;
        case ATMOSPHERE_PRESET_STORMY:
            definition->model = ATMOSPHERE_MODEL_PREETHAM;
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
