#include "auto.h"

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "render.h"
#include "textures.h"
#include "scenery.h"
#include "system.h"
#include "water.h"
#include "zone.h"

void autoGenRealisticLandscape(int seed)
{
    WaterDefinition water;
    TexturesDefinition textures;
    TextureLayerDefinition* texture;
    int layer;

    if (!seed)
    {
        seed = time(NULL);
    }
    srand(seed);

    /* Cloud layer */
    CloudsDefinition* clouds = CloudsDefinitionClass.create();
    layer = layersAddLayer(clouds->layers, NULL);
    scenerySetClouds(clouds);
    CloudsDefinitionClass.destroy(clouds);

    /* Water */
    water = waterCreateDefinition();
    waterAutoPreset(&water, WATER_PRESET_LAKE);
    scenerySetWater(&water);
    waterDeleteDefinition(&water);

    /* Textures */
    textures = texturesCreateDefinition();
    layer = layersAddLayer(textures.layers, NULL);
    layersSetName(textures.layers, layer, "Ground");
    texture = layersGetLayer(textures.layers, layer);
    noiseClearLevels(texture->bump_noise);
    noiseAddLevelsSimple(texture->bump_noise, 8, 1.0, -0.5, 0.5);
    texture->bump_height = 0.01;
    texture->bump_scaling = 0.045;
    texture->material.base.r = 0.6;
    texture->material.base.g = 0.55;
    texture->material.base.b = 0.57;
    texture->material.reflection = 0.02;
    texture->material.shininess = 3.0;
    texture->thickness = 0.001;
    texture->slope_range = 0.001;
    texture->thickness_transparency = 0.0;
    layer = layersAddLayer(textures.layers, NULL);
    layersSetName(textures.layers, layer, "Grass");
    texture = layersGetLayer(textures.layers, layer);
    zoneAddHeightRangeQuick(texture->zone, 1.0, -6.0, -5.0, 3.0, 15.0);
    zoneAddSlopeRangeQuick(texture->zone, 1.0, 0.0, 0.0, 0.05, 0.4);
    noiseClearLevels(texture->bump_noise);
    noiseAddLevelsSimple(texture->bump_noise, 5, 1.0, -0.2, 0.2);
    noiseAddLevelsSimple(texture->bump_noise, 2, 0.03, -0.04, 0.04);
    texture->bump_height = 0.002;
    texture->bump_scaling = 0.03;
    texture->material.base.r = 0.12;
    texture->material.base.g = 0.19;
    texture->material.base.b = 0.035;
    texture->material.reflection = 0.003;
    texture->material.shininess = 2.0;
    texture->thickness = 0.02;
    texture->slope_range = 0.03;
    texture->thickness_transparency = 0.005;
    /*texture = texturesGetLayer(&textures, texturesAddLayer(&textures));
    zoneAddHeightRangeQuick(texture->zone, 1.0, 4.0, 5.0, 100.0, 100.0);
    zoneAddSlopeRangeQuick(texture->zone, 1.0, 0.0, 0.0, 0.2, 1.0);
    noiseGenerateBaseNoise(texture->bump_noise, 102400);
    noiseClearLevels(texture->bump_noise);
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
    /*atmosphere = atmosphereCreateDefinition();
    atmosphere.distance_near = 20.0;
    atmosphere.distance_far = 100.0;
    atmosphere.full_mask = 0.6;
    atmosphere.auto_lock_on_haze = 1;
    scenerySetAtmosphere(&atmosphere);
    atmosphereDeleteDefinition(&atmosphere);*/
}
