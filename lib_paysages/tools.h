#ifndef _PAYSAGES_TOOLS_H_
#define _PAYSAGES_TOOLS_H_

#include "shared/types.h"
#include "tools/euclid.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UNUSED(_x_) ((void)(_x_))

double toolsRandom();
double toolsCubicInterpolate(double stencil[4], double x);
double toolsBicubicInterpolate(double stencil[16], double x, double y);
void toolsFloat2DMapCopy(double* src, double* dest, int src_xstart, int src_ystart, int dest_xstart, int dest_ystart, int xsize, int ysize, int src_xstep, int src_ystep, int dest_xstep, int dest_ystep);
Vector3 toolsGetNormalFromTriangle(Vector3 center, Vector3 bottom, Vector3 right);
double toolsGetDistance2D(double x1, double y1, double x2, double y2);

#ifdef __cplusplus
}
#endif

#endif
