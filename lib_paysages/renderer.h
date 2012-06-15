#ifndef _PAYSAGES_RENDERER_H_
#define _PAYSAGES_RENDERER_H_

#include "shared/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Renderer Renderer;

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
    float render_progress;
    int render_interrupt;
    int is_rendering;
    float (*getPrecision)(Renderer* renderer, Vector3 location);
    Vector3 (*projectPoint)(Renderer* renderer, Vector3 point);
    Vector3 (*unprojectPoint)(Renderer* renderer, Vector3 point);
    int (*addRenderProgress)(Renderer* renderer, float progress);
    void (*pushTriangle)(Renderer* renderer, Vertex* v1, Vertex* v2, Vertex* v3);
    void (*pushQuad)(Renderer* renderer, Vertex* v1, Vertex* v2, Vertex* v3, Vertex* v4);

    /* Scenery related */
    RayCastingResult (*rayWalking)(Renderer* renderer, Vector3 location, Vector3 direction, int terrain, int water, int sky, int clouds);
    float (*getTerrainHeight)(Renderer* renderer, float x, float z);
    HeightInfo (*getWaterHeightInfo)(Renderer* renderer);
    Color (*applyTextures)(Renderer* renderer, Vector3 location, float precision);
    Color (*applyAtmosphere)(Renderer* renderer, Vector3 location, Color base);
    Color (*applyClouds)(Renderer* renderer, Color base, Vector3 start, Vector3 end);

    /* Lighting related */
    void (*alterLight)(Renderer* renderer, LightDefinition* light, Vector3 location);
    void (*getLightStatus)(Renderer* renderer, LightStatus* status, Vector3 location);
    Color (*applyLightStatus)(Renderer* renderer, LightStatus* status, Vector3 location, Vector3 normal, SurfaceMaterial material);

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
