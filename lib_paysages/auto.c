#include "auto.h"

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "shared/types.h"
#include "shared/constants.h"
#include "clouds.h"
#include "color.h"
#include "lighting.h"
#include "modifiers.h"
#include "render.h"
#include "terrain.h"
#include "textures.h"
#include "scenery.h"
#include "sky.h"
#include "system.h"
#include "water.h"
#include "zone.h"

void autoSetDaytime(int hour, int minute)
{
    autoSetDaytimeFraction((double)hour / 24.0 + (double)minute / 1440.0);
}

void autoSetDaytimeFraction(double daytime)
{
    SkyDefinition sky;
    /*ColorGradation grad_sun;
    Color sun;*/

    daytime = fmod(daytime, 1.0);
    if (daytime < 0.0)
    {
        daytime += 1.0;
    }

    /*lightingSetSunAngle(0.0, (daytime + 0.25) * M_PI * 2.0);

    grad_sun = colorGradationCreate();
    colorGradationAddRgba(&grad_sun, 0.2, 0.1, 0.1, 0.1, 1.0);
    colorGradationAddRgba(&grad_sun, 0.25, 0.9, 0.5, 0.5, 1.0);
    colorGradationAddRgba(&grad_sun, 0.3, 0.8, 0.8, 0.8, 1.0);
    colorGradationAddRgba(&grad_sun, 0.5, 1.0, 1.0, 1.0, 1.0);
    colorGradationAddRgba(&grad_sun, 0.7, 0.8, 0.8, 0.8, 1.0);
    colorGradationAddRgba(&grad_sun, 0.75, 0.7, 0.6, 0.5, 1.0);
    colorGradationAddRgba(&grad_sun, 0.8, 0.1, 0.1, 0.1, 1.0);
    sun = colorGradationGet(&grad_sun, daytime);
    lightingSetSunColor(sun);*/

    sky = skyCreateDefinition();
    sceneryGetSky(&sky);
    sky.daytime = daytime;
    scenerySetSky(&sky);
    skyDeleteDefinition(&sky);
}

void autoGenRealisticLandscape(int seed)
{
    AtmosphereDefinition atmosphere;
    TerrainDefinition terrain;
    WaterDefinition water;
    CloudsDefinition clouds;
    SkyDefinition sky;
    TexturesDefinition textures;
    TextureLayerDefinition* texture;
    LightingDefinition lighting;

    if (!seed)
    {
        seed = time(NULL);
    }
    srand(seed);

    /* Cloud layer */
    clouds = cloudsCreateDefinition();
    cloudsAddLayer(&clouds);
    scenerySetClouds(&clouds);
    cloudsDeleteDefinition(&clouds);

    /* Water */
    water = waterCreateDefinition();
    water.height = 0.0;
    water.transparency = 0.5;
    water.reflection = 0.3;
    water.transparency_depth = 6.0;
    water.material.base.r = 0.1;
    water.material.base.g = 0.3;
    water.material.base.b = 0.4;
    water.material.base.a = 1.0;
    water.material.reflection = 1.0;
    water.material.shininess = 12.0;
    water.depth_color.r = 0.0;
    water.depth_color.g = 0.2;
    water.depth_color.b = 0.3;
    water.depth_color.a = 1.0;
    water.lighting_depth = 3.0;
    water.waves_noise_height = 0.005;
    water.waves_noise_scale = 0.07;
    noiseGenerateBaseNoise(water.waves_noise, 262144);
    noiseAddLevelsSimple(water.waves_noise, 2, 1.0, 1.0);
    noiseAddLevelsSimple(water.waves_noise, 3, 0.15, 0.1);
    scenerySetWater(&water);
    waterDeleteDefinition(&water);

    /* Sky */
    sky = skyCreateDefinition();
    colorGradationQuickAddRgb(sky.sun_color, 0.3, 1.0, 0.91, 0.8);
    colorGradationQuickAddRgb(sky.sun_color, 0.5, 1.0, 0.95, 0.9);
    colorGradationQuickAddRgb(sky.sun_color, 0.7, 1.0, 0.91, 0.8);
    colorGradationQuickAddRgb(sky.zenith_color, 0.2, 0.03, 0.03, 0.05);
    colorGradationQuickAddRgb(sky.zenith_color, 0.25, 0.25, 0.33, 0.37);
    colorGradationQuickAddRgb(sky.zenith_color, 0.35, 0.52, 0.63, 0.8);
    colorGradationQuickAddRgb(sky.zenith_color, 0.65, 0.52, 0.63, 0.8);
    colorGradationQuickAddRgb(sky.zenith_color, 0.75, 0.25, 0.33, 0.37);
    colorGradationQuickAddRgb(sky.zenith_color, 0.8, 0.03, 0.03, 0.05);
    colorGradationQuickAddRgb(sky.haze_color, 0.2, 0.05, 0.05, 0.08);
    colorGradationQuickAddRgb(sky.haze_color, 0.25, 0.55, 0.42, 0.42);
    colorGradationQuickAddRgb(sky.haze_color, 0.3, 0.6, 0.6, 0.6);
    colorGradationQuickAddRgb(sky.haze_color, 0.4, 0.92, 0.93, 1.0);
    colorGradationQuickAddRgb(sky.haze_color, 0.6, 0.92, 0.93, 1.0);
    colorGradationQuickAddRgb(sky.haze_color, 0.7, 0.6, 0.6, 0.8);
    colorGradationQuickAddRgb(sky.haze_color, 0.75, 0.62, 0.50, 0.42);
    colorGradationQuickAddRgb(sky.haze_color, 0.8, 0.05, 0.05, 0.08);
    sky.daytime = 0.0;
    sky.haze_height = 0.75;
    sky.haze_smoothing = 0.3;
    sky.sun_radius = 0.02;
    scenerySetSky(&sky);
    skyDeleteDefinition(&sky);

    /* Lighting */
    lighting = lightingCreateDefinition();
    lighting.autosetfromsky = 1;
    scenerySetLighting(&lighting);
    lightingDeleteDefinition(&lighting);

    /* Terrain */
    terrain = terrainCreateDefinition();
    noiseGenerateBaseNoise(terrain.height_noise, 1048576);
    noiseAddLevelsSimple(terrain.height_noise, 12, 1.0, 1.0);
    terrain.height_factor = 12.0 / noiseGetMaxValue(terrain.height_noise);
    terrain.scaling = 10.0;
    scenerySetTerrain(&terrain);
    terrainDeleteDefinition(&terrain);

    /* Textures */
    textures = texturesCreateDefinition();
    texture = texturesGetLayer(&textures, texturesAddLayer(&textures));
    zoneAddHeightRangeQuick(texture->zone, 1.0, -20.0, -20.0, 20.0, 20.0);
    zoneAddSlopeRangeQuick(texture->zone, 1.0, 0.0, 0.0, 5.0, 5.0);
    noiseGenerateBaseNoise(texture->bump_noise, 102400);
    noiseAddLevelsSimple(texture->bump_noise, 8, 1.0, 1.0);
    texture->bump_height = 0.01;
    texture->bump_scaling = 0.015;
    texture->material.base.r = 0.6;
    texture->material.base.g = 0.55;
    texture->material.base.b = 0.57;
    texture->material.reflection = 0.2;
    texture->material.shininess = 3.0;
    texture->thickness = 0.001;
    texture->slope_range = 0.001;
    texture->thickness_transparency = 0.0;
    texture = texturesGetLayer(&textures, texturesAddLayer(&textures));
    zoneAddHeightRangeQuick(texture->zone, 1.0, -1.0, 0.0, 3.0, 15.0);
    zoneAddSlopeRangeQuick(texture->zone, 1.0, 0.0, 0.0, 0.1, 0.7);
    noiseGenerateBaseNoise(texture->bump_noise, 102400);
    noiseAddLevelsSimple(texture->bump_noise, 5, 1.0, 0.4);
    noiseAddLevelsSimple(texture->bump_noise, 2, 0.03, 0.08);
    texture->bump_height = 0.002;
    texture->bump_scaling = 0.01;
    texture->material.base.r = 0.12;
    texture->material.base.g = 0.19;
    texture->material.base.b = 0.035;
    texture->material.reflection = 0.1;
    texture->material.shininess = 2.0;
    texture->thickness = 0.02;
    texture->slope_range = 0.03;
    texture->thickness_transparency = 0.005;
    /*texture = texturesGetLayer(&textures, texturesAddLayer(&textures));
    zoneAddHeightRangeQuick(texture->zone, 1.0, 4.0, 5.0, 100.0, 100.0);
    zoneAddSlopeRangeQuick(texture->zone, 1.0, 0.0, 0.0, 0.2, 1.0);
    noiseGenerateBaseNoise(texture->bump_noise, 102400);
    noiseAddLevelsSimple(texture->bump_noise, 6, 1.0, 1.0);
    texture->bump_height = 0.002;
    texture->bump_scaling = 0.03;
    texture->material.base.r = 1.0;
    texture->material.base.g = 1.0;
    texture->material.base.b = 1.0;
    texture->material.reflection = 0.25;
    texture->material.shininess = 0.6;
    texture->thickness = 0.05;
    texture->slope_range = 0.3;
    texture->thickness_transparency = 0.015;*/
    scenerySetTextures(&textures);
    texturesDeleteDefinition(&textures);

    /* Atmosphere */
    atmosphere = atmosphereCreateDefinition();
    atmosphere.distance_near = 20.0;
    atmosphere.distance_far = 100.0;
    atmosphere.full_mask = 0.6;
    atmosphere.auto_lock_on_haze = 1;
    scenerySetAtmosphere(&atmosphere);
    atmosphereDeleteDefinition(&atmosphere);
}
