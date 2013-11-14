#ifndef SCENERY_H
#define SCENERY_H

#include "rendering_global.h"

#include "BaseDefinition.h"

class AtmosphereDefinition;
class CloudsDefinition;
class TerrainDefinition;
class TexturesDefinition;
class Renderer;

typedef void (*SceneryCustomDataCallback)(PackStream* stream, void* data);

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

    static Scenery* getCurrent();

    void setCustomSaveCallbacks(SceneryCustomDataCallback callback_save, SceneryCustomDataCallback callback_load, void* data);

    virtual void save(PackStream* stream) const override;
    virtual void load(PackStream* stream) override;

    virtual void validate() override;

    void autoPreset(int seed);

    void setAtmosphere(AtmosphereDefinition* atmosphere);
    inline AtmosphereDefinition* getAtmosphere() const {return atmosphere;}
    void getAtmosphere(AtmosphereDefinition* atmosphere);

    void setCamera(CameraDefinition* camera);
    inline CameraDefinition* getCamera() const {return camera;}
    void getCamera(CameraDefinition* camera);

    void setClouds(CloudsDefinition* clouds);
    inline CloudsDefinition* getClouds() const {return clouds;}
    void getClouds(CloudsDefinition* clouds);

    void setTerrain(TerrainDefinition* terrain);
    inline TerrainDefinition* getTerrain() const {return terrain;}
    void getTerrain(TerrainDefinition* terrain);

    void setTextures(TexturesDefinition* textures);
    inline TexturesDefinition* getTextures() const {return textures;}
    void getTextures(TexturesDefinition* textures);

    void setWater(WaterDefinition* water);
    inline WaterDefinition* getWater() const {return water;}
    void getWater(WaterDefinition* water);

    void bindToRenderer(Renderer* renderer);

    void checkCameraAboveGround();

private:
    AtmosphereDefinition* atmosphere;
    CameraDefinition* camera;
    CloudsDefinition* clouds;
    TerrainDefinition* terrain;
    TexturesDefinition* textures;
    WaterDefinition* water;

    SceneryCustomDataCallback _custom_save;
    SceneryCustomDataCallback _custom_load;
    void* _custom_data;
};

// Transitional C-API

RENDERINGSHARED_EXPORT void sceneryRenderFirstPass(Renderer* renderer);

#endif // SCENERY_H
