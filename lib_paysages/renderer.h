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
    void (*pushTriangle)(Renderer* renderer, Vertex* v1, Vertex* v2, Vertex* v3);
    void (*pushQuad)(Renderer* renderer, Vertex* v1, Vertex* v2, Vertex* v3, Vertex* v4);

    /* Scenery related */
    RayCastingResult (*rayWalking)(Renderer* renderer, Vector3 location, Vector3 direction, int terrain, int water, int sky, int clouds);
    double (*getTerrainHeight)(Renderer* renderer, double x, double z);
    HeightInfo (*getWaterHeightInfo)(Renderer* renderer);
    Color (*applyTextures)(Renderer* renderer, Vector3 location, double precision);
    Color (*applyAtmosphere)(Renderer* renderer, Vector3 location, Color base);
    Color (*applyClouds)(Renderer* renderer, Color base, Vector3 start, Vector3 end);

    /* Lighting related */
    Color (*filterLight)(Renderer* renderer, Color light_color, Vector3 at_location, Vector3 light_location, Vector3 direction_to_light);
    Color (*maskLight)(Renderer* renderer, Color light_color, Vector3 at_location, Vector3 light_location, Vector3 direction_to_light);
    Color (*applyLightingToSurface)(Renderer* renderer, Vector3 location, Vector3 normal, SurfaceMaterial material);

    /* Custom data */
    void* customData[10];
};

Renderer rendererCreate();
void rendererDelete(Renderer* renderer);
void rendererSetPreviewCallbacks(Renderer* renderer, RenderCallbackStart start, RenderCallbackDraw draw, RenderCallbackUpdate update);
void rendererStart(Renderer* renderer, int width, int height, int quality);
void rendererInterrupt(Renderer* renderer);

#ifdef __cplusplus
}
#endif

#endif
