#include "tools.h"

#include <assert.h>
#include <stdlib.h>
#include <inttypes.h>

#include "color.h"
#include "euclid.h"

#define pack754_32(f) (pack754((f), 32, 8))
#define pack754_64(f) (pack754((f), 64, 11))
#define unpack754_32(i) (unpack754((i), 32, 8))
#define unpack754_64(i) (unpack754((i), 64, 11))

static uint64_t pack754(double f, unsigned bits, unsigned expbits)
{
    double fnorm;
    int shift;
    long long sign, exp, significand;
    unsigned significandbits = bits - expbits - 1; // -1 for sign bit

    if (f == 0.0) return 0; // get this special case out of the way

    // check sign and begin normalization
    if (f < 0) { sign = 1; fnorm = -f; }
    else { sign = 0; fnorm = f; }

    // get the normalized form of f and track the exponent
    shift = 0;
    while(fnorm >= 2.0) { fnorm /= 2.0; shift++; }
    while(fnorm < 1.0) { fnorm *= 2.0; shift--; }
    fnorm = fnorm - 1.0;

    // calculate the binary form (non-float) of the significand data
    significand = fnorm * ((1LL<<significandbits) + 0.5f);

    // get the biased exponent
    exp = shift + ((1<<(expbits-1)) - 1); // shift + bias

    // return the final answer
    return (sign<<(bits-1)) | (exp<<(bits-expbits-1)) | significand;
}

static double unpack754(uint64_t i, unsigned bits, unsigned expbits)
{
    double result;
    long long shift;
    unsigned bias;
    unsigned significandbits = bits - expbits - 1; // -1 for sign bit

    if (i == 0) return 0.0;

    // pull the significand
    result = (i&((1LL<<significandbits)-1)); // mask
    result /= (1LL<<significandbits); // convert back to float
    result += 1.0f; // add the one back on

    // deal with the exponent
    bias = (1<<(expbits-1)) - 1;
    shift = ((i>>significandbits)&((1LL<<expbits)-1)) - bias;
    while(shift > 0) { result *= 2.0; shift--; }
    while(shift < 0) { result /= 2.0; shift++; }

    // sign it
    result *= (i>>(bits-1))&1? -1.0: 1.0;

    return result;
}

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
    uint64_t servalue;
    
    servalue = pack754_64(*value);
    fwrite(&servalue, sizeof(uint64_t), 1, f);
}

void toolsLoadDouble(FILE* f, double* value)
{
    int read;
    uint64_t servalue;
    
    read = fread(&servalue, sizeof(uint64_t), 1, f);
    assert(read == 1);
    
    *value = unpack754_64(servalue);
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
