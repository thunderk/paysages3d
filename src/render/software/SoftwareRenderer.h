#ifndef SOFTWARERENDERER_H
#define SOFTWARERENDERER_H

#include "software_global.h"

namespace paysages {
namespace software {

/**
 * This class renders a defined scenery in sotware mode (using only standard CPU computations).
 */
class SOFTWARESHARED_EXPORT SoftwareRenderer {

  public:
    SoftwareRenderer(Scenery *scenery);
    virtual ~SoftwareRenderer();

    /* Render base configuration */
    int render_quality;
    CameraDefinition *render_camera;

    virtual Vector3 getCameraLocation();
    virtual Vector3 getCameraDirection();
    virtual double getPrecision(const Vector3 &location);
    virtual Vector3 projectPoint(const Vector3 &point);
    virtual Vector3 unprojectPoint(const Vector3 &point);

    /**
     * Prepare the renderer sub-systems.
     *
     * This will clear the caches and connect elements together.
     * After this call, don't update the scenery when renderer is in use.
     */
    virtual void prepare();

    /**
     * Set the global quality control factor.
     *
     * Values between 0.0 and 1.0 are standard quality (1.0 is considered a "very good" production quality value).
     *
     * Values above 1.0 are used for boosting ("extra" quality, for demanding renders).
     */
    virtual void setQuality(double quality);

    inline Scenery *getScenery() const {
        return scenery;
    }

    inline BaseAtmosphereRenderer *getAtmosphereRenderer() const {
        return atmosphere_renderer;
    }
    inline CloudsRenderer *getCloudsRenderer() const {
        return clouds_renderer;
    }
    inline TerrainRenderer *getTerrainRenderer() const {
        return terrain_renderer;
    }
    inline TexturesRenderer *getTexturesRenderer() const {
        return textures_renderer;
    }
    inline WaterRenderer *getWaterRenderer() const {
        return water_renderer;
    }
    inline VegetationRenderer *getVegetationRenderer() const {
        return vegetation_renderer;
    }

    inline NightSky *getNightSky() const {
        return nightsky_renderer;
    }

    inline FluidMediumManager *getFluidMediumManager() const {
        return fluid_medium;
    }
    inline LightingManager *getLightingManager() const {
        return lighting;
    }
    inline GodRaysSampler *getGodRaysSampler() const {
        return godrays;
    }

    virtual Color applyLightingToSurface(const Vector3 &location, const Vector3 &normal,
                                         const SurfaceMaterial &material);
    virtual Color applyMediumTraversal(const Vector3 &location, const Color &color);
    virtual RayCastingResult rayWalking(const Vector3 &location, const Vector3 &direction, int terrain, int water,
                                        int sky, int clouds);

  private:
    Scenery *scenery;

    FluidMediumManager *fluid_medium;
    LightingManager *lighting;
    GodRaysSampler *godrays;

    BaseAtmosphereRenderer *atmosphere_renderer;
    CloudsRenderer *clouds_renderer;
    TerrainRenderer *terrain_renderer;
    TexturesRenderer *textures_renderer;
    WaterRenderer *water_renderer;
    NightSky *nightsky_renderer;
    VegetationRenderer *vegetation_renderer;
};
}
}

#endif // SOFTWARERENDERER_H
