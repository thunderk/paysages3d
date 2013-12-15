#ifndef SOFTWARERENDERER_H
#define SOFTWARERENDERER_H

#include "software_global.h"

#include "RenderArea.h"
#include "RayCastingManager.h"

namespace paysages {
namespace software {

/*!
 * \brief This class renders a defined scenery in sotware mode (using only standard CPU computations).
 */
class SOFTWARESHARED_EXPORT SoftwareRenderer
{

public:
    SoftwareRenderer(Scenery* scenery=0);
    virtual ~SoftwareRenderer();

    /* Render base configuration */
    int render_quality;
    int render_width;
    int render_height;
    CameraDefinition* render_camera;

    /* Render related */
    RenderArea* render_area;
    double render_progress;
    int render_interrupt;
    int is_rendering;

    void* customData[10];

    virtual Vector3 getCameraLocation(const Vector3 &target);
    virtual Vector3 getCameraDirection(const Vector3 &target);
    virtual double getPrecision(const Vector3 &location);
    virtual Vector3 projectPoint(const Vector3 &point);
    virtual Vector3 unprojectPoint(const Vector3 &point);
    virtual int addRenderProgress(double progress);
    virtual void pushTriangle(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3, RenderArea::f_RenderFragmentCallback callback, void* callback_data);
    virtual void pushQuad(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3, const Vector3 &v4, RenderArea::f_RenderFragmentCallback callback, void* callback_data);
    virtual void pushDisplacedTriangle(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3, const Vector3 &ov1, const Vector3 &ov2, const Vector3 &ov3, RenderArea::f_RenderFragmentCallback callback, void* callback_data);
    virtual void pushDisplacedQuad(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3, const Vector3 &v4, const Vector3 &ov1, const Vector3 &ov2, const Vector3 &ov3, const Vector3 &ov4, RenderArea::f_RenderFragmentCallback callback, void* callback_data);

    /*!
     * \brief Set the scenery to render.
     *
     * Don't call this after rendering has already started.
     */
    virtual void setScenery(Scenery* scenery);

    /*!
     * \brief Prepare the renderer sub-systems.
     *
     * This will clear the caches and connect elements together.
     * After this call, don't update the scenery when renderer is in use.
     */
    virtual void prepare();

    /*!
     * \brief Start the rasterization process.
     */
    virtual void rasterize();

    /*!
     * \brief Disable the clouds feature.
     *
     * This toggle is permanent, provided the clouds part of the sceney is not changed.
     */
    void disableClouds();
    /*!
     * \brief Disable atmosphere and sky lighting, replacing it by static lights.
     *
     * This function needs to be called after each prepare().
     */
    void disableAtmosphere(const std::vector<LightComponent> &lights);

    void setPreviewCallbacks(RenderArea::RenderCallbackStart start, RenderArea::RenderCallbackDraw draw, RenderArea::RenderCallbackUpdate update);
    void start(RenderArea::RenderParams params);
    void interrupt();

    inline Scenery* getScenery() const {return scenery;}

    inline BaseAtmosphereRenderer* getAtmosphereRenderer() const {return atmosphere_renderer;}
    inline CloudsRenderer* getCloudsRenderer() const {return clouds_renderer;}
    inline TerrainRenderer* getTerrainRenderer() const {return terrain_renderer;}
    inline TexturesRenderer* getTexturesRenderer() const {return textures_renderer;}
    inline WaterRenderer* getWaterRenderer() const {return water_renderer;}

    inline FluidMediumManager* getFluidMediumManager() const {return fluid_medium;}
    inline LightingManager* getLightingManager() const {return lighting;}

    virtual Color applyLightingToSurface(const Vector3 &location, const Vector3 &normal, const SurfaceMaterial &material);
    virtual Color applyMediumTraversal(Vector3 location, Color color);
    virtual RayCastingResult rayWalking(const Vector3 &location, const Vector3 &direction, int terrain, int water, int sky, int clouds);

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
