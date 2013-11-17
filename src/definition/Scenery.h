#ifndef SCENERY_H
#define SCENERY_H

#include "definition_global.h"

#include "BaseDefinition.h"

namespace paysages {
namespace definition {

/**
 * @brief Global scenery management
 *
 * This class contains the whole scenery definition.
 */
class DEFINITIONSHARED_EXPORT Scenery: public BaseDefinition
{
public:
    Scenery();

    virtual void save(PackStream* stream) const override;
    virtual void load(PackStream* stream) override;

    virtual void validate() override;
    virtual Scenery* getScenery() override;

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

    void checkCameraAboveGround();

private:
    AtmosphereDefinition* atmosphere;
    CameraDefinition* camera;
    CloudsDefinition* clouds;
    TerrainDefinition* terrain;
    TexturesDefinition* textures;
    WaterDefinition* water;
};

}
}

#endif // SCENERY_H
