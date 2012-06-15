#include "tools.h"

#include <assert.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>

#include "color.h"
#include "euclid.h"

float toolsRandom()
{
    return (float)rand() / (float)RAND_MAX;
}

static inline float __cubicInterpolate(float p[4], float x)
{
    return p[1] + 0.5 * x * (p[2] - p[0] + x * (2.0 * p[0] - 5.0 * p[1] + 4.0 * p[2] - p[3] + x * (3.0 * (p[1] - p[2]) + p[3] - p[0])));
}

float toolsBicubicInterpolate(float stencil[16], float x, float y)
{
    float buf_cubic_y[4];

    buf_cubic_y[0] = __cubicInterpolate(stencil, x);
    buf_cubic_y[1] = __cubicInterpolate(stencil + 4, x);
    buf_cubic_y[2] = __cubicInterpolate(stencil + 8, x);
    buf_cubic_y[3] = __cubicInterpolate(stencil + 12, x);

    return __cubicInterpolate(buf_cubic_y, y);
}

void toolsFloat2DMapCopy(float* src, float* dest, int src_xstart, int src_ystart, int dest_xstart, int dest_ystart, int xsize, int ysize, int src_xstep, int src_ystep, int dest_xstep, int dest_ystep)
{
    /* TODO Optimize with memcpy if src_xstep == dest_xstep == 1 */
    int x, y;
    float* src_row;
    float* dest_row;
    src += src_ystart * src_ystep + src_xstart * src_xstep;
    dest += dest_ystart * dest_ystep + dest_xstart * dest_xstep;
    for (y = 0; y < ysize; y++)
    {
       src_row = src;
       dest_row = dest;
       for (x = 0; x < xsize; x++)
       {
           *dest = *src;
           src += src_xstep;
           dest += dest_xstep;
       }
       src = src_row + src_ystep;
       dest = dest_row + dest_ystep;
    }
}

Vector3 toolsGetNormalFromTriangle(Vector3 center, Vector3 bottom, Vector3 right)
{
    Vector3 dx = v3Sub(right, center);
    Vector3 dz = v3Sub(bottom, center);
    return v3Normalize(v3Cross(dz, dx));
}

float toolsGetDistance2D(float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

void materialSave(PackStream* stream, SurfaceMaterial* material)
{
    colorSave(stream, &material->base);
    packWriteFloat(stream, &material->reflection);
    packWriteFloat(stream, &material->shininess);
}

void materialLoad(PackStream* stream, SurfaceMaterial* material)
{
    colorLoad(stream, &material->base);
    packReadFloat(stream, &material->reflection);
    packReadFloat(stream, &material->shininess);
}
