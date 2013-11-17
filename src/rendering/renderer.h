#ifndef _PAYSAGES_RENDERER_H_
#define _PAYSAGES_RENDERER_H_

#include "rendering_global.h"
#include "shared/types.h"
#include "render.h"

class LightingManager;
class AtmosphereRenderer;
class TerrainRenderer;
class TexturesRenderer;
class CloudsRenderer;
class WaterRenderer;

class Renderer
{
public:
    Renderer();
    virtual ~Renderer();

    virtual void prepare() {}
    virtual void setScenery(Scenery*) {}

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
    Vector3(*getCameraLocation)(Renderer* renderer, Vector3 target);
    Vector3(*getCameraDirection)(Renderer* renderer, Vector3 target);
    double (*getPrecision)(Renderer* renderer, Vector3 location);
    Vector3(*projectPoint)(Renderer* renderer, Vector3 point);
    Vector3(*unprojectPoint)(Renderer* renderer, Vector3 point);
    int (*addRenderProgress)(Renderer* renderer, double progress);
    void (*pushTriangle)(Renderer* renderer, Vector3 v1, Vector3 v2, Vector3 v3, f_RenderFragmentCallback callback, void* callback_data);
    void (*pushQuad)(Renderer* renderer, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, f_RenderFragmentCallback callback, void* callback_data);
    void (*pushDisplacedTriangle)(Renderer* renderer, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 ov1, Vector3 ov2, Vector3 ov3, f_RenderFragmentCallback callback, void* callback_data);
    void (*pushDisplacedQuad)(Renderer* renderer, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Vector3 ov1, Vector3 ov2, Vector3 ov3, Vector3 ov4, f_RenderFragmentCallback callback, void* callback_data);

    /* Shortcuts */
    Color(*applyLightingToSurface)(Renderer* renderer, Vector3 location, Vector3 normal, SurfaceMaterial* material);
    virtual Color applyMediumTraversal(Vector3 location, Color color);

    /* Scenery related */
    RayCastingResult(*rayWalking)(Renderer* renderer, Vector3 location, Vector3 direction, int terrain, int water, int sky, int clouds);

    /* Autonomous tools */
    LightingManager* lighting;

    /* Autonomous sub-renderers */
    AtmosphereRenderer* atmosphere;
    TerrainRenderer* terrain;
    TexturesRenderer* textures;
    CloudsRenderer* clouds;
    WaterRenderer* water;

    /* Custom data */
    void* customData[10];
};

RENDERINGSHARED_EXPORT Renderer* rendererCreate();
RENDERINGSHARED_EXPORT void rendererDelete(Renderer* renderer);
RENDERINGSHARED_EXPORT void rendererSetPreviewCallbacks(Renderer* renderer, RenderCallbackStart start, RenderCallbackDraw draw, RenderCallbackUpdate update);
RENDERINGSHARED_EXPORT void rendererStart(Renderer* renderer, RenderParams params);
RENDERINGSHARED_EXPORT void rendererInterrupt(Renderer* renderer);

#endif
