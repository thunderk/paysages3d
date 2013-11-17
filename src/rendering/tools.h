#ifndef _PAYSAGES_TOOLS_H_
#define _PAYSAGES_TOOLS_H_

#include "rendering_global.h"
#include "shared/types.h"

#define UNUSED(_x_) ((void)(_x_))

RENDERINGSHARED_EXPORT double toolsRandom();
RENDERINGSHARED_EXPORT void toolsFloat2DMapCopy(double* src, double* dest, int src_xstart, int src_ystart, int dest_xstart, int dest_ystart, int xsize, int ysize, int src_xstep, int src_ystep, int dest_xstep, int dest_ystep);

#endif
