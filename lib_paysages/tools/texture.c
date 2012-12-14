#include "texture.h"

#include <assert.h>
#include <math.h>
#include "../system.h"

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
    /* TODO */
    return texture2DGetNearest(tex, dx, dy);
}

Color texture2DGetCubic(Texture2D* tex, double dx, double dy)
{
    /* TODO */
    return texture2DGetNearest(tex, dx, dy);
}

void texture2DSaveToFile(Texture2D* tex, const char* filepath)
{
    systemSavePictureFile(filepath, (PictureCallbackSavePixel)texture2DGetPixel, tex, tex->xsize, tex->ysize);
}

static void _callbackTex2dLoad(Texture2D* tex, int x, int y, Color col)
{
    if (x >= 0 && x < tex->xsize && y >= 0 && y < tex->ysize)
    {
        tex->data[y * tex->xsize + x] = col;
    }
}

void texture2DLoadFromFile(Texture2D* tex, const char* filepath)
{
    systemLoadPictureFile(filepath, NULL, (PictureCallbackLoadPixel)_callbackTex2dLoad, tex);
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
    /* TODO */
    return texture3DGetNearest(tex, dx, dy, dz);
}

Color texture3DGetCubic(Texture3D* tex, double dx, double dy, double dz)
{
    /* TODO */
    return texture3DGetNearest(tex, dx, dy, dz);
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

static void _callbackTex3dLoad(Texture3D* tex, int x, int y, Color col)
{
    int z = y / tex->ysize;
    y = y % tex->ysize;

    if (x >= 0 && x < tex->xsize && y >= 0 && y < tex->ysize && z >= 0 && z < tex->zsize)
    {
        tex->data[z * tex->xsize * tex->ysize + y * tex->xsize + x] = col;
    }
}

void texture3DLoadFromFile(Texture3D* tex, const char* filepath)
{
    systemLoadPictureFile(filepath, NULL, (PictureCallbackLoadPixel)_callbackTex3dLoad, tex);
}
