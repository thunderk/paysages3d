#ifndef _PAYSAGES_TOOLS_TEXTURE_H_
#define _PAYSAGES_TOOLS_TEXTURE_H_

/*
 * Pixel based textures.
 */

#include "../rendering_global.h"
#include "color.h"

namespace paysages {
namespace system {
class PackStream;
}
}

typedef struct Texture2D Texture2D;
typedef struct Texture3D Texture3D;
typedef struct Texture4D Texture4D;

RENDERINGSHARED_EXPORT Texture2D* texture2DCreate(int xsize, int ysize);
RENDERINGSHARED_EXPORT void texture2DDelete(Texture2D* tex);
RENDERINGSHARED_EXPORT void texture2DGetSize(Texture2D* tex, int* xsize, int* ysize);
RENDERINGSHARED_EXPORT void texture2DSetPixel(Texture2D* tex, int x, int y, Color col);
RENDERINGSHARED_EXPORT Color texture2DGetPixel(Texture2D* tex, int x, int y);
RENDERINGSHARED_EXPORT Color texture2DGetNearest(Texture2D* tex, double dx, double dy);
RENDERINGSHARED_EXPORT Color texture2DGetLinear(Texture2D* tex, double dx, double dy);
RENDERINGSHARED_EXPORT Color texture2DGetCubic(Texture2D* tex, double dx, double dy);
RENDERINGSHARED_EXPORT void texture2DFill(Texture2D* tex, Color col);
RENDERINGSHARED_EXPORT void texture2DAdd(Texture2D* source, Texture2D* destination);
RENDERINGSHARED_EXPORT void texture2DSave(PackStream* stream, Texture2D* tex);
RENDERINGSHARED_EXPORT void texture2DLoad(PackStream* stream, Texture2D* tex);
RENDERINGSHARED_EXPORT void texture2DSaveToFile(Texture2D* tex, const char* filepath);

RENDERINGSHARED_EXPORT Texture3D* texture3DCreate(int xsize, int ysize, int zsize);
RENDERINGSHARED_EXPORT void texture3DDelete(Texture3D* tex);
RENDERINGSHARED_EXPORT void texture3DGetSize(Texture3D* tex, int* xsize, int* ysize, int* zsize);
RENDERINGSHARED_EXPORT void texture3DSetPixel(Texture3D* tex, int x, int y, int z, Color col);
RENDERINGSHARED_EXPORT Color texture3DGetPixel(Texture3D* tex, int x, int y, int z);
RENDERINGSHARED_EXPORT Color texture3DGetNearest(Texture3D* tex, double dx, double dy, double dz);
RENDERINGSHARED_EXPORT Color texture3DGetLinear(Texture3D* tex, double dx, double dy, double dz);
RENDERINGSHARED_EXPORT Color texture3DGetCubic(Texture3D* tex, double dx, double dy, double dz);
RENDERINGSHARED_EXPORT void texture3DFill(Texture3D* tex, Color col);
RENDERINGSHARED_EXPORT void texture3DAdd(Texture3D* source, Texture3D* destination);
RENDERINGSHARED_EXPORT void texture3DSave(PackStream* stream, Texture3D* tex);
RENDERINGSHARED_EXPORT void texture3DLoad(PackStream* stream, Texture3D* tex);
RENDERINGSHARED_EXPORT void texture3DSaveToFile(Texture3D* tex, const char* filepath);

RENDERINGSHARED_EXPORT Texture4D* texture4DCreate(int xsize, int ysize, int zsize, int wsize);
RENDERINGSHARED_EXPORT void texture4DDelete(Texture4D* tex);
RENDERINGSHARED_EXPORT void texture4DGetSize(Texture4D* tex, int* xsize, int* ysize, int* zsize, int* wsize);
RENDERINGSHARED_EXPORT void texture4DSetPixel(Texture4D* tex, int x, int y, int z, int w, Color col);
RENDERINGSHARED_EXPORT Color texture4DGetPixel(Texture4D* tex, int x, int y, int z, int w);
RENDERINGSHARED_EXPORT Color texture4DGetNearest(Texture4D* tex, double dx, double dy, double dz, double dw);
RENDERINGSHARED_EXPORT Color texture4DGetLinear(Texture4D* tex, double dx, double dy, double dz, double dw);
RENDERINGSHARED_EXPORT Color texture4DGetCubic(Texture4D* tex, double dx, double dy, double dz, double dw);
RENDERINGSHARED_EXPORT void texture4DFill(Texture4D* tex, Color col);
RENDERINGSHARED_EXPORT void texture4DAdd(Texture4D* source, Texture4D* destination);
RENDERINGSHARED_EXPORT void texture4DSave(PackStream* stream, Texture4D* tex);
RENDERINGSHARED_EXPORT void texture4DLoad(PackStream* stream, Texture4D* tex);
RENDERINGSHARED_EXPORT void texture4DSaveToFile(Texture4D* tex, const char* filepath);

#endif
