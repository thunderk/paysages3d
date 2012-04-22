#ifndef _PAYSAGES_TOOLS_H_
#define _PAYSAGES_TOOLS_H_

#include "pack.h"
#include "shared/types.h"

#ifdef __cplusplus
extern "C" {
#endif

double toolsRandom();
double toolsBicubicInterpolate(double stencil[16], double x, double y);
void toolsFloat2DMapCopy(double* src, double* dest, int src_xstart, int src_ystart, int dest_xstart, int dest_ystart, int xsize, int ysize, int src_xstep, int src_ystep, int dest_xstep, int dest_ystep);
Vector3 toolsGetNormalFromTriangle(Vector3 center, Vector3 bottom, Vector3 right);
double toolsGetDistance2D(double x1, double y1, double x2, double y2);
void materialSave(PackStream* stream, SurfaceMaterial* material);
void materialLoad(PackStream* stream, SurfaceMaterial* material);

#ifdef __cplusplus
}
#endif

#endif
