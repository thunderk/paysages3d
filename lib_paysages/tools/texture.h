#ifndef _PAYSAGES_TOOLS_TEXTURE_H_
#define _PAYSAGES_TOOLS_TEXTURE_H_

/*
 * Pixel based textures.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "../color.h"

typedef struct Texture2D Texture2D;
typedef struct Texture3D Texture3D;

Texture2D* texture2DCreate(int xsize, int ysize);
void texture2DDelete(Texture2D* tex);
void texture2DGetSize(Texture2D* tex, int* xsize, int* ysize);
void texture2DSetPixel(Texture2D* tex, int x, int y, Color col);
Color texture2DGetPixel(Texture2D* tex, int x, int y);
Color texture2DGetNearest(Texture2D* tex, double dx, double dy);
Color texture2DGetLinear(Texture2D* tex, double dx, double dy);
Color texture2DGetCubic(Texture2D* tex, double dx, double dy);
void texture2DSaveToFile(Texture2D* tex, const char* filepath);
void texture2DLoadFromFile(Texture2D* tex, const char* filepath);

Texture3D* texture3DCreate(int xsize, int ysize, int zsize);
void texture3DDelete(Texture3D* tex);
void texture3DGetSize(Texture3D* tex, int* xsize, int* ysize, int* zsize);
void texture3DSetPixel(Texture3D* tex, int x, int y, int z, Color col);
Color texture3DGetPixel(Texture3D* tex, int x, int y, int z);
Color texture3DGetNearest(Texture3D* tex, double dx, double dy, double dz);
Color texture3DGetLinear(Texture3D* tex, double dx, double dy, double dz);
Color texture3DGetCubic(Texture3D* tex, double dx, double dy, double dz);
void texture3DSaveToFile(Texture3D* tex, const char* filepath);
void texture3DLoadFromFile(Texture3D* tex, const char* filepath);

#ifdef __cplusplus
}
#endif

#endif
