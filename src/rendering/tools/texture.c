#include "texture.h"

#include <assert.h>
#include <math.h>
#include "../system.h"
#include "PictureFile.h"

struct Texture2D
{
    int xsize;
    int ysize;
    Color* data;
};

struct Texture3D
{
    int xsize;
    int ysize;
    int zsize;
    Color* data;
};

struct Texture4D
{
    int xsize;
    int ysize;
    int zsize;
    int wsize;
    Color* data;
};



static inline Color _lerp(Color c1, Color c2, double d)
{
    Color result;

    result.r = c1.r * (1.0 - d) + c2.r * d;
    result.g = c1.g * (1.0 - d) + c2.g * d;
    result.b = c1.b * (1.0 - d) + c2.b * d;
    result.a = c1.a * (1.0 - d) + c2.a * d;

    return result;
}


Texture2D* texture2DCreate(int xsize, int ysize)
{
    Texture2D* result;

    assert(xsize > 0 && ysize > 0);

    result = (Texture2D*)malloc(sizeof(Texture2D));
    result->xsize = xsize;
    result->ysize = ysize;
    result->data = malloc(sizeof(Color) * xsize * ysize);

    return result;
}

void texture2DDelete(Texture2D* tex)
{
    free(tex->data);
    free(tex);
}

void texture2DGetSize(Texture2D* tex, int* xsize, int* ysize)
{
    *xsize = tex->xsize;
    *ysize = tex->ysize;
}

void texture2DSetPixel(Texture2D* tex, int x, int y, Color col)
{
    assert(x >= 0 && x < tex->xsize);
    assert(y >= 0 && y < tex->ysize);

    tex->data[y * tex->xsize + x] = col;
}

Color texture2DGetPixel(Texture2D* tex, int x, int y)
{
    assert(x >= 0 && x < tex->xsize);
    assert(y >= 0 && y < tex->ysize);

    return tex->data[y * tex->xsize + x];
}

Color texture2DGetNearest(Texture2D* tex, double dx, double dy)
{
    if (dx < 0.0) dx = 0.0;
    if (dx > 1.0) dx = 1.0;
    if (dy < 0.0) dy = 0.0;
    if (dy > 1.0) dy = 1.0;

    int ix = (int)(dx * (double)(tex->xsize - 1));
    int iy = (int)(dy * (double)(tex->ysize - 1));

    assert(ix >= 0 && ix < tex->xsize);
    assert(iy >= 0 && iy < tex->ysize);

    return tex->data[iy * tex->xsize + ix];
}

Color texture2DGetLinear(Texture2D* tex, double dx, double dy)
{
    if (dx < 0.0) dx = 0.0;
    if (dx > 1.0) dx = 1.0;
    if (dy < 0.0) dy = 0.0;
    if (dy > 1.0) dy = 1.0;

    dx *= (double)(tex->xsize - 1);
    dy *= (double)(tex->ysize - 1);

    int ix = (int)floor(dx);
    if (ix == tex->xsize - 1)
    {
        ix--;
    }
    int iy = (int)floor(dy);
    if (iy == tex->ysize - 1)
    {
        iy--;
    }

    dx -= (double)ix;
    dy -= (double)iy;

    Color* data = tex->data + iy * tex->xsize + ix;

    Color c1 = _lerp(*data, *(data + 1), dx);
    Color c2 = _lerp(*(data + tex->xsize), *(data + tex->xsize + 1), dx);

    return _lerp(c1, c2, dy);
}

Color texture2DGetCubic(Texture2D* tex, double dx, double dy)
{
    /* TODO */
    return texture2DGetLinear(tex, dx, dy);
}

void texture2DFill(Texture2D* tex, Color col)
{
    int i, n;
    n = tex->xsize * tex->ysize;
    for (i = 0; i < n; i++)
    {
        tex->data[i] = col;
    }
}

void texture2DAdd(Texture2D* source, Texture2D* destination)
{
    int i, n;

    assert(source->xsize == destination->xsize);
    assert(source->ysize == destination->ysize);

    n = source->xsize * source->ysize;
    for (i = 0; i < n; i++)
    {
        destination->data[i].r += source->data[i].r;
        destination->data[i].g += source->data[i].g;
        destination->data[i].b += source->data[i].b;
        /* destination->data[i].a += source->data[i].a; */
    }
}

void texture2DSave(PackStream* stream, Texture2D* tex)
{
    int i, n;
    packWriteInt(stream, &tex->xsize);
    packWriteInt(stream, &tex->ysize);
    n = tex->xsize * tex->ysize;
    for (i = 0; i < n; i++)
    {
        colorSave(stream, tex->data + i);
    }
}

void texture2DLoad(PackStream* stream, Texture2D* tex)
{
    int i, n;
    packReadInt(stream, &tex->xsize);
    packReadInt(stream, &tex->ysize);
    n = tex->xsize * tex->ysize;
    tex->data = realloc(tex->data, sizeof(Color) * n);
    for (i = 0; i < n; i++)
    {
        colorLoad(stream, tex->data + i);
    }
}

void texture2DSaveToFile(Texture2D* tex, const char* filepath)
{
    systemSavePictureFile(filepath, (PictureCallbackSavePixel)texture2DGetPixel, tex, tex->xsize, tex->ysize);
}




Texture3D* texture3DCreate(int xsize, int ysize, int zsize)
{
    Texture3D* result;

    assert(xsize > 0 && ysize > 0 && zsize > 0);

    result = (Texture3D*)malloc(sizeof(Texture3D));
    result->xsize = xsize;
    result->ysize = ysize;
    result->zsize = zsize;
    result->data = malloc(sizeof(Color) * xsize * ysize * zsize);

    return result;
}

void texture3DDelete(Texture3D* tex)
{
    free(tex->data);
    free(tex);
}

void texture3DGetSize(Texture3D* tex, int* xsize, int* ysize, int* zsize)
{
    *xsize = tex->xsize;
    *ysize = tex->ysize;
    *zsize = tex->zsize;
}

void texture3DSetPixel(Texture3D* tex, int x, int y, int z, Color col)
{
    assert(x >= 0 && x < tex->xsize);
    assert(y >= 0 && y < tex->ysize);
    assert(z >= 0 && z < tex->ysize);

    tex->data[z * tex->xsize * tex->ysize + y * tex->xsize + x] = col;
}

Color texture3DGetPixel(Texture3D* tex, int x, int y, int z)
{
    assert(x >= 0 && x < tex->xsize);
    assert(y >= 0 && y < tex->ysize);
    assert(z >= 0 && z < tex->zsize);

    return tex->data[z * tex->xsize * tex->ysize + y * tex->xsize + x];
}

Color texture3DGetNearest(Texture3D* tex, double dx, double dy, double dz)
{
    if (dx < 0.0) dx = 0.0;
    if (dx > 1.0) dx = 1.0;
    if (dy < 0.0) dy = 0.0;
    if (dy > 1.0) dy = 1.0;
    if (dz < 0.0) dz = 0.0;
    if (dz > 1.0) dz = 1.0;

    int ix = (int)(dx * (double)(tex->xsize - 1));
    int iy = (int)(dy * (double)(tex->ysize - 1));
    int iz = (int)(dz * (double)(tex->zsize - 1));

    assert(ix >= 0 && ix < tex->xsize);
    assert(iy >= 0 && iy < tex->ysize);
    assert(iz >= 0 && iz < tex->zsize);

    return tex->data[iz * tex->xsize * tex->ysize + iy * tex->xsize + ix];
}

Color texture3DGetLinear(Texture3D* tex, double dx, double dy, double dz)
{
    if (dx < 0.0) dx = 0.0;
    if (dx > 1.0) dx = 1.0;
    if (dy < 0.0) dy = 0.0;
    if (dy > 1.0) dy = 1.0;
    if (dz < 0.0) dz = 0.0;
    if (dz > 1.0) dz = 1.0;

    dx *= (double)(tex->xsize - 1);
    dy *= (double)(tex->ysize - 1);
    dz *= (double)(tex->zsize - 1);

    int ix = (int)floor(dx);
    if (ix == tex->xsize - 1)
    {
        ix--;
    }
    int iy = (int)floor(dy);
    if (iy == tex->ysize - 1)
    {
        iy--;
    }
    int iz = (int)floor(dz);
    if (iz == tex->zsize - 1)
    {
        iz--;
    }

    dx -= (double)ix;
    dy -= (double)iy;
    dz -= (double)iz;

    Color* data = tex->data + iz * tex->xsize * tex->ysize + iy * tex->xsize + ix;

    Color cx1 = _lerp(*data, *(data + 1), dx);
    Color cx2 = _lerp(*(data + tex->xsize), *(data + tex->xsize + 1), dx);
    Color cy1 = _lerp(cx1, cx2, dy);

    data += tex->xsize * tex->ysize;
    cx1 = _lerp(*(data), *(data + 1), dx);
    cx2 = _lerp(*(data + tex->xsize), *(data + tex->xsize + 1), dx);
    Color cy2 = _lerp(cx1, cx2, dy);

    return _lerp(cy1, cy2, dz);
}

Color texture3DGetCubic(Texture3D* tex, double dx, double dy, double dz)
{
    /* TODO */
    return texture3DGetLinear(tex, dx, dy, dz);
}

void texture3DFill(Texture3D* tex, Color col)
{
    int i, n;
    n = tex->xsize * tex->ysize * tex->zsize;
    for (i = 0; i < n; i++)
    {
        tex->data[i] = col;
    }
}

void texture3DAdd(Texture3D* source, Texture3D* destination)
{
    int i, n;

    assert(source->xsize == destination->xsize);
    assert(source->ysize == destination->ysize);
    assert(source->zsize == destination->zsize);

    n = source->xsize * source->ysize * source->zsize;
    for (i = 0; i < n; i++)
    {
        destination->data[i].r += source->data[i].r;
        destination->data[i].g += source->data[i].g;
        destination->data[i].b += source->data[i].b;
        /* destination->data[i].a += source->data[i].a; */
    }
}

void texture3DSave(PackStream* stream, Texture3D* tex)
{
    int i, n;
    packWriteInt(stream, &tex->xsize);
    packWriteInt(stream, &tex->ysize);
    packWriteInt(stream, &tex->zsize);
    n = tex->xsize * tex->ysize * tex->zsize;
    for (i = 0; i < n; i++)
    {
        colorSave(stream, tex->data + i);
    }
}

void texture3DLoad(PackStream* stream, Texture3D* tex)
{
    int i, n;
    packReadInt(stream, &tex->xsize);
    packReadInt(stream, &tex->ysize);
    packReadInt(stream, &tex->zsize);
    n = tex->xsize * tex->ysize * tex->zsize;
    tex->data = realloc(tex->data, sizeof(Color) * n);
    for (i = 0; i < n; i++)
    {
        colorLoad(stream, tex->data + i);
    }
}

static Color _callbackTex3dSave(Texture3D* tex, int x, int y)
{
    int z = y / tex->ysize;
    y = y % tex->ysize;

    assert(x >= 0 && x < tex->xsize);
    assert(y >= 0 && y < tex->ysize);
    assert(z >= 0 && z < tex->zsize);

    return tex->data[z * tex->xsize * tex->ysize + y * tex->xsize + x];
}

void texture3DSaveToFile(Texture3D* tex, const char* filepath)
{
    systemSavePictureFile(filepath, (PictureCallbackSavePixel)_callbackTex3dSave, tex, tex->xsize, tex->ysize * tex->zsize);
}



Texture4D* texture4DCreate(int xsize, int ysize, int zsize, int wsize)
{
    Texture4D* result;

    assert(xsize > 0 && ysize > 0 && zsize > 0 && wsize > 0);

    result = (Texture4D*)malloc(sizeof(Texture4D));
    result->xsize = xsize;
    result->ysize = ysize;
    result->zsize = zsize;
    result->wsize = wsize;
    result->data = malloc(sizeof(Color) * xsize * ysize * zsize * wsize);

    return result;
}

void texture4DDelete(Texture4D* tex)
{
    free(tex->data);
    free(tex);
}

void texture4DGetSize(Texture4D* tex, int* xsize, int* ysize, int* zsize, int* wsize)
{
    *xsize = tex->xsize;
    *ysize = tex->ysize;
    *zsize = tex->zsize;
    *wsize = tex->wsize;
}

void texture4DSetPixel(Texture4D* tex, int x, int y, int z, int w, Color col)
{
    assert(x >= 0 && x < tex->xsize);
    assert(y >= 0 && y < tex->ysize);
    assert(z >= 0 && z < tex->zsize);
    assert(w >= 0 && w < tex->wsize);

    tex->data[w * tex->xsize * tex->ysize * tex->zsize + z * tex->xsize * tex->ysize + y * tex->xsize + x] = col;
}

Color texture4DGetPixel(Texture4D* tex, int x, int y, int z, int w)
{
    assert(x >= 0 && x < tex->xsize);
    assert(y >= 0 && y < tex->ysize);
    assert(z >= 0 && z < tex->zsize);
    assert(w >= 0 && w < tex->wsize);

    return tex->data[w * tex->xsize * tex->ysize * tex->zsize + z * tex->xsize * tex->ysize + y * tex->xsize + x];
}

Color texture4DGetNearest(Texture4D* tex, double dx, double dy, double dz, double dw)
{
    if (dx < 0.0) dx = 0.0;
    if (dx > 1.0) dx = 1.0;
    if (dy < 0.0) dy = 0.0;
    if (dy > 1.0) dy = 1.0;
    if (dz < 0.0) dz = 0.0;
    if (dz > 1.0) dz = 1.0;
    if (dw < 0.0) dw = 0.0;
    if (dw > 1.0) dw = 1.0;

    int ix = (int)(dx * (double)(tex->xsize - 1));
    int iy = (int)(dy * (double)(tex->ysize - 1));
    int iz = (int)(dz * (double)(tex->zsize - 1));
    int iw = (int)(dw * (double)(tex->wsize - 1));

    assert(ix >= 0 && ix < tex->xsize);
    assert(iy >= 0 && iy < tex->ysize);
    assert(iz >= 0 && iz < tex->zsize);
    assert(iw >= 0 && iw < tex->wsize);

    return tex->data[iw * tex->xsize * tex->ysize * tex->zsize + iz * tex->xsize * tex->ysize + iy * tex->xsize + ix];
}

Color texture4DGetLinear(Texture4D* tex, double dx, double dy, double dz, double dw)
{
    if (dx < 0.0) dx = 0.0;
    if (dx > 1.0) dx = 1.0;
    if (dy < 0.0) dy = 0.0;
    if (dy > 1.0) dy = 1.0;
    if (dz < 0.0) dz = 0.0;
    if (dz > 1.0) dz = 1.0;
    if (dw < 0.0) dw = 0.0;
    if (dw > 1.0) dw = 1.0;

    dx *= (double)(tex->xsize - 1);
    dy *= (double)(tex->ysize - 1);
    dz *= (double)(tex->zsize - 1);
    dw *= (double)(tex->wsize - 1);

    int ix = (int)floor(dx);
    if (ix == tex->xsize - 1)
    {
        ix--;
    }
    int iy = (int)floor(dy);
    if (iy == tex->ysize - 1)
    {
        iy--;
    }
    int iz = (int)floor(dz);
    if (iz == tex->zsize - 1)
    {
        iz--;
    }
    int iw = (int)floor(dw);
    if (iw == tex->wsize - 1)
    {
        iw--;
    }

    dx -= (double)ix;
    dy -= (double)iy;
    dz -= (double)iz;
    dw -= (double)iw;

    Color* data = tex->data + iw * tex->xsize * tex->ysize * tex->zsize + iz * tex->xsize * tex->ysize + iy * tex->xsize + ix;
    Color cz1, cz2;

    Color cx1 = _lerp(*data, *(data + 1), dx);
    Color cx2 = _lerp(*(data + tex->xsize), *(data + tex->xsize + 1), dx);
    Color cy1 = _lerp(cx1, cx2, dy);

    data += tex->xsize * tex->ysize;
    cx1 = _lerp(*(data), *(data + 1), dx);
    cx2 = _lerp(*(data + tex->xsize), *(data + tex->xsize + 1), dx);
    Color cy2 = _lerp(cx1, cx2, dy);

    data += tex->xsize * tex->ysize * tex->zsize;
    cz1 = _lerp(cy1, cy2, dz);

    cx1 = _lerp(*data, *(data + 1), dx);
    cx2 = _lerp(*(data + tex->xsize), *(data + tex->xsize + 1), dx);
    cy1 = _lerp(cx1, cx2, dy);

    cx1 = _lerp(*(data), *(data + 1), dx);
    cx2 = _lerp(*(data + tex->xsize), *(data + tex->xsize + 1), dx);
    cy2 = _lerp(cx1, cx2, dy);

    cz2 = _lerp(cy1, cy2, dz);

    return _lerp(cz1, cz2, dw);
}

Color texture4DGetCubic(Texture4D* tex, double dx, double dy, double dz, double dw)
{
    /* TODO */
    return texture4DGetLinear(tex, dx, dy, dz, dw);
}

void texture4DFill(Texture4D* tex, Color col)
{
    int i, n;
    n = tex->xsize * tex->ysize * tex->zsize * tex->wsize;
    for (i = 0; i < n; i++)
    {
        tex->data[i] = col;
    }
}

void texture4DAdd(Texture4D* source, Texture4D* destination)
{
    int i, n;

    assert(source->xsize == destination->xsize);
    assert(source->ysize == destination->ysize);
    assert(source->zsize == destination->zsize);
    assert(source->wsize == destination->wsize);

    n = source->xsize * source->ysize * source->zsize * source->wsize;
    for (i = 0; i < n; i++)
    {
        destination->data[i].r += source->data[i].r;
        destination->data[i].g += source->data[i].g;
        destination->data[i].b += source->data[i].b;
        /* destination->data[i].a += source->data[i].a; */
    }
}

void texture4DSave(PackStream* stream, Texture4D* tex)
{
    int i, n;
    packWriteInt(stream, &tex->xsize);
    packWriteInt(stream, &tex->ysize);
    packWriteInt(stream, &tex->zsize);
    packWriteInt(stream, &tex->wsize);
    n = tex->xsize * tex->ysize * tex->zsize * tex->wsize;
    for (i = 0; i < n; i++)
    {
        colorSave(stream, tex->data + i);
    }
}

void texture4DLoad(PackStream* stream, Texture4D* tex)
{
    int i, n;
    packReadInt(stream, &tex->xsize);
    packReadInt(stream, &tex->ysize);
    packReadInt(stream, &tex->zsize);
    packReadInt(stream, &tex->wsize);
    n = tex->xsize * tex->ysize * tex->zsize * tex->wsize;
    tex->data = realloc(tex->data, sizeof(Color) * n);
    for (i = 0; i < n; i++)
    {
        colorLoad(stream, tex->data + i);
    }
}

static Color _callbackTex4dSave(Texture4D* tex, int x, int y)
{
    int w = x / tex->xsize;
    x = x % tex->xsize;

    int z = y / tex->ysize;
    y = y % tex->ysize;

    assert(x >= 0 && x < tex->xsize);
    assert(y >= 0 && y < tex->ysize);
    assert(z >= 0 && z < tex->zsize);
    assert(w >= 0 && w < tex->wsize);

    return tex->data[w * tex->xsize * tex->ysize * tex->zsize + z * tex->xsize * tex->ysize + y * tex->xsize + x];
}

void texture4DSaveToFile(Texture4D* tex, const char* filepath)
{
    systemSavePictureFile(filepath, (PictureCallbackSavePixel)_callbackTex4dSave, tex, tex->xsize * tex->wsize, tex->ysize * tex->zsize);
}
