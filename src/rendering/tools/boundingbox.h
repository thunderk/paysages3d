#ifndef _RENDERING_TOOLS_BOUNDINGBOX_H_
#define _RENDERING_TOOLS_BOUNDINGBOX_H_

#include "../rendering_global.h"
#include "euclid.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    int empty;
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    double zmin;
    double zmax;
} BoundingBox;

RENDERINGSHARED_EXPORT void boundingBoxReset(BoundingBox* box);
RENDERINGSHARED_EXPORT void boundingBoxPushPoint(BoundingBox* box, Vector3 point);

#ifdef __cplusplus
}
#endif

#endif
