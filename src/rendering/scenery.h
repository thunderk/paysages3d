#ifndef _PAYSAGES_SCENERY_H_
#define _PAYSAGES_SCENERY_H_

/*
 * Scenery management.
 *
 * This module handles all scenery components (terrain, water...) definitions and maintains
 * a standard renderer.
 */

#include "rendering_global.h"
#include "tools/pack.h"
#include "atmosphere/public.h"
#include "clouds/public.h"
#include "terrain/public.h"
#include "textures/public.h"
#include "water/public.h"
#include "camera.h"
#include "renderer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*SceneryCustomDataCallback)(PackStream* stream, void* data);

RENDERINGSHARED_EXPORT void sceneryInit();
RENDERINGSHARED_EXPORT void sceneryQuit();

RENDERINGSHARED_EXPORT void sceneryAutoPreset(int seed);

RENDERINGSHARED_EXPORT void scenerySetCustomDataCallback(SceneryCustomDataCallback callback_save, SceneryCustomDataCallback callback_load, void* data);

RENDERINGSHARED_EXPORT void scenerySave(PackStream* stream);
RENDERINGSHARED_EXPORT void sceneryLoad(PackStream* stream);

RENDERINGSHARED_EXPORT void scenerySetAtmosphere(AtmosphereDefinition* atmosphere);
RENDERINGSHARED_EXPORT void sceneryGetAtmosphere(AtmosphereDefinition* atmosphere);

RENDERINGSHARED_EXPORT void scenerySetCamera(CameraDefinition* camera);
RENDERINGSHARED_EXPORT void sceneryGetCamera(CameraDefinition* camera);

RENDERINGSHARED_EXPORT void scenerySetClouds(CloudsDefinition* clouds);
RENDERINGSHARED_EXPORT void sceneryGetClouds(CloudsDefinition* clouds);

RENDERINGSHARED_EXPORT void scenerySetTerrain(TerrainDefinition* terrain);
RENDERINGSHARED_EXPORT void sceneryGetTerrain(TerrainDefinition* terrain);
RENDERINGSHARED_EXPORT TerrainDefinition* sceneryGetTerrainDirect();

RENDERINGSHARED_EXPORT void scenerySetTextures(TexturesDefinition* textures);
RENDERINGSHARED_EXPORT void sceneryGetTextures(TexturesDefinition* textures);

RENDERINGSHARED_EXPORT void scenerySetWater(WaterDefinition* water);
RENDERINGSHARED_EXPORT void sceneryGetWater(WaterDefinition* water);

RENDERINGSHARED_EXPORT Renderer* sceneryCreateStandardRenderer();
RENDERINGSHARED_EXPORT void sceneryBindRenderer(Renderer* renderer);
RENDERINGSHARED_EXPORT void sceneryRenderFirstPass(Renderer* renderer);

#ifdef __cplusplus
}
#endif

#endif
