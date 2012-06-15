#ifndef _PAYSAGES_TOOLS_H_
#define _PAYSAGES_TOOLS_H_

#include "pack.h"
#include "shared/types.h"

#ifdef __cplusplus
extern "C" {
#endif

float toolsRandom();
float toolsBicubicInterpolate(float stencil[16], float x, float y);
void toolsFloat2DMapCopy(float* src, float* dest, int src_xstart, int src_ystart, int dest_xstart, int dest_ystart, int xsize, int ysize, int src_xstep, int src_ystep, int dest_xstep, int dest_ystep);
Vector3 toolsGetNormalFromTriangle(Vector3 center, Vector3 bottom, Vector3 right);
float toolsGetDistance2D(float x1, float y1, float x2, float y2);
void materialSave(PackStream* stream, SurfaceMaterial* material);
void materialLoad(PackStream* stream, SurfaceMaterial* material);

#ifdef __cplusplus
}
#endif

#endif
