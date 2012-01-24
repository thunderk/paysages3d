#ifndef _PAYSAGES_SCENERY_H_
#define _PAYSAGES_SCENERY_H_

/*
 * Scenery management.
 *
 * This module handles all scenery components (terrain, water...) definitions and maintains
 * a standard renderer.
 */

#include "camera.h"
#include "clouds.h"
#include "lighting.h"
#include "sky.h"
#include "terrain.h"
#include "textures.h"
#include "water.h"

#include "renderer.h"

#ifdef __cplusplus
extern "C" {
#endif

void sceneryInit();
void scenerySaveToFile(char* filepath);
void sceneryLoadFromFile(char* filepath);

void scenerySetCamera(CameraDefinition* camera);
void sceneryGetCamera(CameraDefinition* camera);

void scenerySetClouds(CloudsDefinition* clouds);
void sceneryGetClouds(CloudsDefinition* clouds);

void scenerySetLighting(LightingDefinition* lighting);
void sceneryGetLighting(LightingDefinition* lighting);

void scenerySetSky(SkyDefinition* sky);
void sceneryGetSky(SkyDefinition* sky);

void scenerySetTerrain(TerrainDefinition* terrain);
void sceneryGetTerrain(TerrainDefinition* terrain);

void scenerySetTextures(TexturesDefinition* textures);
void sceneryGetTextures(TexturesDefinition* textures);

void scenerySetWater(WaterDefinition* water);
void sceneryGetWater(WaterDefinition* water);

Renderer sceneryGetStandardRenderer(int quality);
void sceneryRenderFirstPass(Renderer* renderer);
void sceneryRenderSecondPass(Renderer* renderer);

#ifdef __cplusplus
}
#endif

#endif
