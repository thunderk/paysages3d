#ifndef _PAYSAGES_SCENERY_H_
#define _PAYSAGES_SCENERY_H_

/*
 * Scenery management.
 *
 * This module handles all scenery components (terrain, water...) definitions and maintains
 * a standard renderer.
 */

#include "atmosphere/atmosphere.h"
#include "camera.h"
#include "clouds.h"
#include "lighting.h"
#include "terrain.h"
#include "textures.h"
#include "water.h"
#include "pack.h"
#include "renderer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*SceneryCustomDataCallback)(PackStream* stream, void* data);
    
void sceneryInit();
void sceneryQuit();

void scenerySetCustomDataCallback(SceneryCustomDataCallback callback_save, SceneryCustomDataCallback callback_load, void* data);

void scenerySave(PackStream* stream);
void sceneryLoad(PackStream* stream);

void scenerySetAtmosphere(AtmosphereDefinition* atmosphere);
void sceneryGetAtmosphere(AtmosphereDefinition* atmosphere);

void scenerySetCamera(CameraDefinition* camera);
void sceneryGetCamera(CameraDefinition* camera);

void scenerySetClouds(CloudsDefinition* clouds);
void sceneryGetClouds(CloudsDefinition* clouds);

void scenerySetLighting(LightingDefinition* lighting);
void sceneryGetLighting(LightingDefinition* lighting);

void scenerySetTerrain(TerrainDefinition* terrain);
void sceneryGetTerrain(TerrainDefinition* terrain);

void scenerySetTextures(TexturesDefinition* textures);
void sceneryGetTextures(TexturesDefinition* textures);

void scenerySetWater(WaterDefinition* water);
void sceneryGetWater(WaterDefinition* water);

Renderer sceneryCreateStandardRenderer();
void sceneryRenderFirstPass(Renderer* renderer);

#ifdef __cplusplus
}
#endif

#endif
