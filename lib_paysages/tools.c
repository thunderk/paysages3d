#include "tools.h"

#include <assert.h>
#include <stdlib.h>

#include "color.h"
#include "euclid.h"

double toolsRandom()
{
    return (double)rand() / (double)RAND_MAX;
}

static inline double __cubicInterpolate(double p[4], double x)
{
    return p[1] + 0.5 * x * (p[2] - p[0] + x * (2.0 * p[0] - 5.0 * p[1] + 4.0 * p[2] - p[3] + x * (3.0 * (p[1] - p[2]) + p[3] - p[0])));
}

double toolsBicubicInterpolate(double stencil[16], double x, double y)
{
    double buf_cubic_y[4];

    buf_cubic_y[0] = __cubicInterpolate(stencil, x);
    buf_cubic_y[1] = __cubicInterpolate(stencil + 4, x);
    buf_cubic_y[2] = __cubicInterpolate(stencil + 8, x);
    buf_cubic_y[3] = __cubicInterpolate(stencil + 12, x);

    return __cubicInterpolate(buf_cubic_y, y);
}

void toolsFloat2DMapCopy(double* src, double* dest, int src_xstart, int src_ystart, int dest_xstart, int dest_ystart, int xsize, int ysize, int src_xstep, int src_ystep, int dest_xstep, int dest_ystep)
{
    /* TODO Optimize with memcpy if src_xstep == dest_xstep == 1 */
    int x, y;
    double* src_row;
    double* dest_row;
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

void toolsSaveDouble(FILE* f, double* value)
{
    fprintf(f, "%.20le;", *value);
}

void toolsLoadDouble(FILE* f, double* value)
{
    int read;
    
    read = fscanf(f, "%le;", value);
    assert(read == 1);
}

void toolsSaveInt(FILE* f, int* value)
{
    fprintf(f, "%d;", *value);
}

void toolsLoadInt(FILE* f, int* value)
{
    int read;
    
    read = fscanf(f, "%d;", value);
    assert(read == 1);
}

void materialSave(FILE* f, SurfaceMaterial* material)
{
    colorSave(f, &material->base);
    toolsSaveDouble(f, &material->reflection);
    toolsSaveDouble(f, &material->shininess);
}

void materialLoad(FILE* f, SurfaceMaterial* material)
{
    colorLoad(f, &material->base);
    toolsLoadDouble(f, &material->reflection);
    toolsLoadDouble(f, &material->shininess);
}
