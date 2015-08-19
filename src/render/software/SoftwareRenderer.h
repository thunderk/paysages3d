#ifndef SOFTWARERENDERER_H
#define SOFTWARERENDERER_H

#include "software_global.h"

#include "RayCastingManager.h"

namespace paysages {
namespace software {

/*!
 * \brief This class renders a defined scenery in sotware mode (using only standard CPU computations).
 */
class SOFTWARESHARED_EXPORT SoftwareRenderer
{

public:
    SoftwareRenderer(Scenery* scenery);
    virtual ~SoftwareRenderer();

    /* Render base configuration */
    int render_quality;
    CameraDefinition* render_camera;

    void* customData[10];

    virtual Vector3 getCameraLocation(const Vector3 &target);
    virtual Vector3 getCameraDirection(const Vector3 &target);
    virtual double getPrecision(const Vector3 &location);
    virtual Vector3 projectPoint(const Vector3 &point);
    virtual Vector3 unprojectPoint(const Vector3 &point);

    /*!
     * \brief Prepare the renderer sub-systems.
     *
     * This will clear the caches and connect elements together.
     * After this call, don't update the scenery when renderer is in use.
     */
    virtual void prepare();

    /*!
     * \brief Disable atmosphere and sky lighting, replacing it by static lights.
     *
     * This function needs to be called after each prepare().
     *
     * WARNING : This method changes the scenery attached to the renderer !
     */
    void disableAtmosphere();
    void disableAtmosphere(const std::vector<LightComponent> &lights);

    inline Scenery* getScenery() const {return scenery;}

    inline BaseAtmosphereRenderer* getAtmosphereRenderer() const {return atmosphere_renderer;}
    inline CloudsRenderer* getCloudsRenderer() const {return clouds_renderer;}
    inline TerrainRenderer* getTerrainRenderer() const {return terrain_renderer;}
    inline TexturesRenderer* getTexturesRenderer() const {return textures_renderer;}
    inline WaterRenderer* getWaterRenderer() const {return water_renderer;}

    inline NightSky* getNightSky() const {return nightsky_renderer;}

    inline FluidMediumManager* getFluidMediumManager() const {return fluid_medium;}
    inline LightingManager* getLightingManager() const {return lighting;}

    virtual Color applyLightingToSurface(const Vector3 &location, const Vector3 &normal, const SurfaceMaterial &material);
    virtual Color applyMediumTraversal(Vector3 location, Color color);
    virtual RayCastingResult rayWalking(const Vector3 &location, const Vector3 &direction, int terrain, int water, int sky, int clouds);

private:
    Scenery* scenery;

    FluidMediumManager* fluid_medium;
    LightingManager* lighting;

    BaseAtmosphereRenderer* atmosphere_renderer;
    CloudsRenderer* clouds_renderer;
    TerrainRenderer* terrain_renderer;
    TexturesRenderer* textures_renderer;
    WaterRenderer* water_renderer;
    NightSky* nightsky_renderer;
};

}
}

#endif // SOFTWARERENDERER_H
