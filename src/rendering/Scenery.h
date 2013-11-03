#ifndef SCENERY_H
#define SCENERY_H

#include "rendering_global.h"

#include "rendering/atmosphere/public.h"
#include "rendering/camera.h"
#include "rendering/clouds/public.h"
#include "rendering/terrain/public.h"
#include "rendering/textures/public.h"
#include "rendering/water/public.h"
#include "PackStream.h"
#include "rendering/renderer.h"

#ifdef __cplusplus

#include "BaseDefinition.h"

//class AtmosphereDefinition;
//class CameraDefinition;
//class CloudsDefinition;
//class TerrainDefinition;
//class TexturesDefinition;
//class WaterDefinition;
//class PackStream;
//class Renderer;

/**
 * @brief Global scenery management
 *
 * This class contains the whole scenery definition.
 */
class RENDERINGSHARED_EXPORT Scenery: private BaseDefinition
{
public:
    Scenery();
    virtual ~Scenery();

    virtual void save(PackStream* stream);
    virtual void load(PackStream* stream);

    void autoPreset(int seed);

    void setAtmosphere(AtmosphereDefinition* atmosphere);
    inline AtmosphereDefinition* getAtmosphere() {return atmosphere;}
    void getAtmosphere(AtmosphereDefinition* atmosphere);

    void setCamera(CameraDefinition* camera);
    inline CameraDefinition* getCamera() {return camera;}
    void getCamera(CameraDefinition* camera);

    void setClouds(CloudsDefinition* clouds);
    inline CloudsDefinition* getClouds() {return clouds;}
    void getClouds(CloudsDefinition* clouds);

    void setTerrain(TerrainDefinition* terrain);
    inline TerrainDefinition* getTerrain() {return terrain;}
    void getTerrain(TerrainDefinition* terrain);

    void setTextures(TexturesDefinition* textures);
    inline TexturesDefinition* getTextures() {return textures;}
    void getTextures(TexturesDefinition* textures);

    void setWater(WaterDefinition* water);
    inline WaterDefinition* getWater() {return water;}
    void getWater(WaterDefinition* water);

    void bindToRenderer(Renderer* renderer);

private:
    AtmosphereDefinition* atmosphere;
    CameraDefinition* camera;
    CloudsDefinition* clouds;
    TerrainDefinition* terrain;
    TexturesDefinition* textures;
    WaterDefinition* water;
};

extern "C" {
#endif

// Transitional C-API
RENDERINGSHARED_EXPORT void sceneryInit();
RENDERINGSHARED_EXPORT void sceneryQuit();

RENDERINGSHARED_EXPORT void sceneryAutoPreset(int seed);

typedef void (*SceneryCustomDataCallback)(PackStream* stream, void* data);
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

#endif // SCENERY_H
