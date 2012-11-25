#ifndef _PAYSAGES_RENDERER_H_
#define _PAYSAGES_RENDERER_H_

#include "shared/types.h"
#include "atmosphere/atmosphere.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Renderer
{
    /* Render base configuration */
    int render_quality;
    int render_width;
    int render_height;
    CameraDefinition render_camera;
    Vector3 camera_location;

    /* Render related */
    RenderArea* render_area;
    double render_progress;
    int render_interrupt;
    int is_rendering;
    double (*getPrecision)(Renderer* renderer, Vector3 location);
    Vector3 (*projectPoint)(Renderer* renderer, Vector3 point);
    Vector3 (*unprojectPoint)(Renderer* renderer, Vector3 point);
    int (*addRenderProgress)(Renderer* renderer, double progress);
    void (*pushTriangle)(Renderer* renderer, Vector3 v1, Vector3 v2, Vector3 v3, f_RenderFragmentCallback callback, void* callback_data);
    void (*pushQuad)(Renderer* renderer, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, f_RenderFragmentCallback callback, void* callback_data);

    /* Scenery related */
    RayCastingResult (*rayWalking)(Renderer* renderer, Vector3 location, Vector3 direction, int terrain, int water, int sky, int clouds);
    double (*getTerrainHeight)(Renderer* renderer, double x, double z);
    HeightInfo (*getWaterHeightInfo)(Renderer* renderer);
    Color (*applyTextures)(Renderer* renderer, Vector3 location, double precision);
    Color (*applyClouds)(Renderer* renderer, Color base, Vector3 start, Vector3 end);

    /* Lighting related */
    void (*alterLight)(Renderer* renderer, LightDefinition* light, Vector3 location);
    void (*getLightStatus)(Renderer* renderer, LightStatus* status, Vector3 location);
    Color (*applyLightStatus)(Renderer* renderer, LightStatus* status, Vector3 location, Vector3 normal, SurfaceMaterial material);
    
    /* Autonomous sub-renderers */
    AtmosphereRenderer* atmosphere;

    /* Custom data */
    void* customData[10];
};

Renderer rendererCreate();
void rendererDelete(Renderer* renderer);
void rendererSetPreviewCallbacks(Renderer* renderer, RenderCallbackStart start, RenderCallbackDraw draw, RenderCallbackUpdate update);
void rendererStart(Renderer* renderer, RenderParams params);
void rendererInterrupt(Renderer* renderer);

#ifdef __cplusplus
}
#endif

#endif
