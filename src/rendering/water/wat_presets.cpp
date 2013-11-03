#include "private.h"

#include "NoiseGenerator.h"

void waterAutoPreset(WaterDefinition* definition, WaterPreset preset)
{
    definition->_waves_noise->randomizeOffsets();

    if (preset == WATER_PRESET_LAKE)
    {
        definition->transparency = 0.5;
        definition->reflection = 0.4;
        definition->transparency_depth = 4.0;
        definition->material.base = colorToHSL(colorFromValues(0.08, 0.15, 0.2, 1.0));
        definition->depth_color.r = 0.0;
        definition->depth_color.g = 0.1;
        definition->depth_color.b = 0.1;
        definition->lighting_depth = 6.0;
        definition->scaling = 1.0;
        definition->waves_height = 0.8;
        definition->detail_height = 0.05;
        definition->turbulence = 0.1;
        definition->foam_coverage = 0.15;
    }
    else if (preset == WATER_PRESET_SEA)
    {
        definition->transparency = 0.4;
        definition->reflection = 0.35;
        definition->transparency_depth = 3.0;
        definition->material.base = colorToHSL(colorFromValues(0.05, 0.18, 0.2, 1.0));
        definition->depth_color.r = 0.0;
        definition->depth_color.g = 0.18;
        definition->depth_color.b = 0.15;
        definition->lighting_depth = 4.0;
        definition->scaling = 1.5;
        definition->waves_height = 1.0;
        definition->detail_height = 0.06;
        definition->turbulence = 0.3;
        definition->foam_coverage = 0.4;
    }

    definition->depth_color.a = 1.0;
    definition->material.base.a = 1.0;
    definition->material.reflection = 1.0;
    definition->material.shininess = 16.0;
    definition->foam_material.base = colorToHSL(colorFromValues(0.8, 0.8, 0.8, 1.0));
    definition->foam_material.reflection = 0.1;
    definition->foam_material.shininess = 1.5;

    WaterDefinitionClass.validate(definition);
}
