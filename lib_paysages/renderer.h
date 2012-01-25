#ifndef _PAYSAGES_RENDERER_H_
#define _PAYSAGES_RENDERER_H_

#include "shared/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Renderer Renderer;

struct Renderer
{
    /* Quality configuration */
    int render_quality;
    Vector3 camera_location;

    /* Render related */
    double (*getPrecision)(Renderer* renderer, Vector3 location);
    Vector3 (*projectPoint)(Renderer* renderer, Vector3 point);
    Vector3 (*unprojectPoint)(Renderer* renderer, Vector3 point);

    /* Scenery related */
    RayCastingResult (*rayWalking)(Renderer* renderer, Vector3 location, Vector3 direction, int terrain, int water, int sky, int clouds);
    double (*getTerrainHeight)(Renderer* renderer, double x, double z);
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

Renderer rendererGetFake();

#ifdef __cplusplus
}
#endif

#endif