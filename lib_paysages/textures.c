#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "shared/types.h"
#include "shared/functions.h"
#include "shared/constants.h"
#include "shared/globals.h"

#include "IL/il.h"
#include "IL/ilu.h"

#define TEXTURES_MAX 50
static int _textures_count = 0;
static Texture _textures[TEXTURES_MAX];

void texturesSave(FILE* f)
{
}

void texturesLoad(FILE* f)
{
}

Texture* textureCreateFromFile(const char* filename)
{
    Texture* result;
    ILuint imageid;

    if (_textures_count >= TEXTURES_MAX)
    {
        return _textures + (TEXTURES_MAX - 1);
    }
    else
    {
        result = _textures + _textures_count;
        _textures_count++;

        ilGenImages(1, &imageid);
        ilBindImage(imageid);
        ilLoadImage(filename);

        result->bytes_per_pixel = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
        result->picture_width = ilGetInteger(IL_IMAGE_WIDTH);
        result->picture_height = ilGetInteger(IL_IMAGE_HEIGHT);
        result->pixels = malloc(result->bytes_per_pixel * result->picture_width * result->picture_height);
        memcpy(result->pixels, ilGetData(), result->bytes_per_pixel * result->picture_width * result->picture_height);
        result->scaling_x = 1.0;
        result->scaling_y = 1.0;
        result->scaling_z = 1.0;

        ilDeleteImages(1, &imageid);

        return result;
    }
}

static inline Color _getRawValue(Texture* tex, int x, int y)
{
    Color result;
    void* texdata;

    x = x % tex->picture_width;
    if (x < 0)
    {
        x += tex->picture_width;
    }
    y = y % tex->picture_height;
    if (y < 0)
    {
        y += tex->picture_height;
    }

    texdata = tex->pixels + (y * tex->picture_width + x) * tex->bytes_per_pixel;

    result.r = ((double)(unsigned int)*((unsigned char*)texdata)) / 255.0;
    result.g = ((double)(unsigned int)*((unsigned char*)(texdata + 1))) / 255.0;
    result.b = ((double)(unsigned int)*((unsigned char*)(texdata + 2))) / 255.0;

    return result;
}

static inline Color _getInterpolatedValue(Texture* tex, double fx, double fy)
{
    Color result;
    double r[16];
    double g[16];
    double b[16];
    int ix, iy;
    int sx, sy;

    ix = (int)floor(fx);
    iy = (int)floor(fy);
    fx -= (double)ix;
    fy -= (double)iy;

    for (sy = 0; sy < 4; sy++)
    {
        for (sx = 0; sx < 4; sx++)
        {
            result = _getRawValue(tex, ix + sx - 1, iy + sy - 1);
            r[sy * 4 + sx] = result.r;
            g[sy * 4 + sx] = result.g;
            b[sy * 4 + sx] = result.b;
        }
    }

    result.r = toolsBicubicInterpolate(r, fx, fy);
    result.g = toolsBicubicInterpolate(g, fx, fy);
    result.b = toolsBicubicInterpolate(b, fx, fy);
    result.a = 1.0;
    return result;
}

Color textureApply(Texture* tex, Vector3 location, Vector3 normal)
{
    Color col_x, col_y, col_z, result;
    double x, y, z;
    double distance;

    distance = v3Norm(v3Sub(camera_location, location)) / 10.0;

    x = location.x / (tex->scaling_x * distance);
    y = location.y / (tex->scaling_y * distance);
    z = location.z / (tex->scaling_z * distance);

    col_x = _getInterpolatedValue(tex, y, z);
    col_y = _getInterpolatedValue(tex, x, z);
    col_z = _getInterpolatedValue(tex, x, y);

    result.r = (col_x.r + col_y.r + col_z.r) / 3.0;
    result.g = (col_x.g + col_y.g + col_z.g) / 3.0;
    result.b = (col_x.b + col_y.b + col_z.b) / 3.0;
    result.a = (col_x.a + col_y.a + col_z.a) / 3.0;
    return result;
}

