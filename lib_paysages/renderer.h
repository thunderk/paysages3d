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

    /* Lighting related */
    Color (*filterLight)(Renderer* renderer, Color light_color, Vector3 at_location, Vector3 light_location, Vector3 direction_to_light);

    /* Custom data */
    void* customData[10];
};

Renderer rendererGetFake();

#ifdef __cplusplus
}
#endif

#endif
