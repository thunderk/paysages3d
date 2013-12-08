#ifndef SOFTWARERENDERER_H
#define SOFTWARERENDERER_H

#include "software_global.h"

#include "renderer.h"

namespace paysages {
namespace software {

/*!
 * \brief This class renders a defined scenery in sotware mode (using only standard CPU computations).
 */
class SOFTWARESHARED_EXPORT SoftwareRenderer: public Renderer
{

public:
    SoftwareRenderer(Scenery* scenery=0);
    virtual ~SoftwareRenderer();

    /*!
     * \brief Set the scenery to render.
     *
     * Don't call this after rendering has already started.
     */
    virtual void setScenery(Scenery* scenery) override;

    /*!
     * \brief Prepare the renderer sub-systems.
     *
     * This will clear the caches and connect elements together.
     * After this call, don't update the scenery when renderer is in use.
     */
    virtual void prepare() override;

    /*!
     * \brief Start the rasterization process.
     */
    virtual void rasterize() override;

    inline Scenery* getScenery() const {return scenery;}

    inline BaseAtmosphereRenderer* getAtmosphereRenderer() const {return atmosphere_renderer;}
    inline CloudsRenderer* getCloudsRenderer() const {return clouds_renderer;}
    inline TerrainRenderer* getTerrainRenderer() const {return terrain_renderer;}
    inline TexturesRenderer* getTexturesRenderer() const {return textures_renderer;}
    inline WaterRenderer* getWaterRenderer() const {return water_renderer;}

    inline FluidMediumManager* getFluidMediumManager() const {return fluid_medium;}
    inline LightingManager* getLightingManager() const {return lighting;}

    virtual Color applyLightingToSurface(const Vector3 &location, const Vector3 &normal, const SurfaceMaterial &material) override;
    virtual Color applyMediumTraversal(Vector3 location, Color color) override;
    virtual RayCastingResult rayWalking(const Vector3 &location, const Vector3 &direction, int terrain, int water, int sky, int clouds) override;

private:
    Scenery* scenery;
    bool own_scenery;

    FluidMediumManager* fluid_medium;
    LightingManager* lighting;

    BaseAtmosphereRenderer* atmosphere_renderer;
    CloudsRenderer* clouds_renderer;
    TerrainRenderer* terrain_renderer;
    TexturesRenderer* textures_renderer;
    WaterRenderer* water_renderer;
};

}
}

#endif // SOFTWARERENDERER_H
