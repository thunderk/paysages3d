#include "noisenaive.h"

/*
 * Naive noise implementation, based on pseudo-random grids.
 */

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "tools.h"

static int _noise_pool_size;
static double* _noise_pool;

void noiseNaiveInit()
{
    int i;

    _noise_pool_size = 1048576;
    _noise_pool = new double[_noise_pool_size];

    for (i = 0; i < _noise_pool_size; i++)
    {
        _noise_pool[i] = toolsRandom() - 0.5;
    }
}

void noiseNaiveQuit()
{
    delete[] _noise_pool;
}

void noiseNaiveSave(PackStream* stream)
{
    int i;

    stream->write(&_noise_pool_size);
    for (i = 0; i < _noise_pool_size; i++)
    {
        stream->write(_noise_pool + i);
    }
}

void noiseNaiveLoad(PackStream* stream)
{
    int i;

    stream->read(&_noise_pool_size);
    delete[] _noise_pool;
    _noise_pool = new double[_noise_pool_size];
    for (i = 0; i < _noise_pool_size; i++)
    {
        stream->read(_noise_pool + i);
    }
}

double noiseNaiveGet1DValue(double x)
{
    x *= 3.0;

    int size = _noise_pool_size;
    int xbase = (int)floor(x);

    double xinternal = x - (double)xbase;

    int x0 = (xbase - 1) % size;
    if (x0 < 0)
    {
        x0 += size;
    }
    int x1 = xbase % size;
    if (x1 < 0)
    {
        x1 += size;
    }
    int x2 = (xbase + 1) % size;
    if (x2 < 0)
    {
        x2 += size;
    }
    int x3 = (xbase + 2) % size;
    if (x3 < 0)
    {
        x3 += size;
    }

    double buf_cubic_x[4];

    buf_cubic_x[0] = _noise_pool[x0 % _noise_pool_size];
    buf_cubic_x[1] = _noise_pool[x1 % _noise_pool_size];
    buf_cubic_x[2] = _noise_pool[x2 % _noise_pool_size];
    buf_cubic_x[3] = _noise_pool[x3 % _noise_pool_size];

    return toolsCubicInterpolate(buf_cubic_x, xinternal) * 0.837 + 0.5;
}

double noiseNaiveGet2DValue(double x, double y)
{
    x *= 3.0;
    y *= 3.0;

    int size = (int)pow(_noise_pool_size, 0.5);

    int xbase = (int)floor(x);
    int ybase = (int)floor(y);

    double xinternal = x - (double)xbase;
    double yinternal = y - (double)ybase;

    int x0 = (xbase - 1) % size;
    if (x0 < 0)
    {
        x0 += size;
    }
    int x1 = xbase % size;
    if (x1 < 0)
    {
        x1 += size;
    }
    int x2 = (xbase + 1) % size;
    if (x2 < 0)
    {
        x2 += size;
    }
    int x3 = (xbase + 2) % size;
    if (x3 < 0)
    {
        x3 += size;
    }

    int y0 = (ybase - 1) % size;
    if (y0 < 0)
    {
        y0 += size;
    }
    int y1 = ybase % size;
    if (y1 < 0)
    {
        y1 += size;
    }
    int y2 = (ybase + 1) % size;
    if (y2 < 0)
    {
        y2 += size;
    }
    int y3 = (ybase + 2) % size;
    if (y3 < 0)
    {
        y3 += size;
    }

    double buf_cubic_x[4];
    double buf_cubic_y[4];

    buf_cubic_x[0] = _noise_pool[(y0 * size + x0) % _noise_pool_size];
    buf_cubic_x[1] = _noise_pool[(y0 * size + x1) % _noise_pool_size];
    buf_cubic_x[2] = _noise_pool[(y0 * size + x2) % _noise_pool_size];
    buf_cubic_x[3] = _noise_pool[(y0 * size + x3) % _noise_pool_size];
    buf_cubic_y[0] = toolsCubicInterpolate(buf_cubic_x, xinternal);

    buf_cubic_x[0] = _noise_pool[(y1 * size + x0) % _noise_pool_size];
    buf_cubic_x[1] = _noise_pool[(y1 * size + x1) % _noise_pool_size];
    buf_cubic_x[2] = _noise_pool[(y1 * size + x2) % _noise_pool_size];
    buf_cubic_x[3] = _noise_pool[(y1 * size + x3) % _noise_pool_size];
    buf_cubic_y[1] = toolsCubicInterpolate(buf_cubic_x, xinternal);

    buf_cubic_x[0] = _noise_pool[(y2 * size + x0) % _noise_pool_size];
    buf_cubic_x[1] = _noise_pool[(y2 * size + x1) % _noise_pool_size];
    buf_cubic_x[2] = _noise_pool[(y2 * size + x2) % _noise_pool_size];
    buf_cubic_x[3] = _noise_pool[(y2 * size + x3) % _noise_pool_size];
    buf_cubic_y[2] = toolsCubicInterpolate(buf_cubic_x, xinternal);

    buf_cubic_x[0] = _noise_pool[(y3 * size + x0) % _noise_pool_size];
    buf_cubic_x[1] = _noise_pool[(y3 * size + x1) % _noise_pool_size];
    buf_cubic_x[2] = _noise_pool[(y3 * size + x2) % _noise_pool_size];
    buf_cubic_x[3] = _noise_pool[(y3 * size + x3) % _noise_pool_size];
    buf_cubic_y[3] = toolsCubicInterpolate(buf_cubic_x, xinternal);

    return toolsCubicInterpolate(buf_cubic_y, yinternal) * 0.723 + 0.5;
}

double noiseNaiveGet3DValue(double x, double y, double z)
{
    x *= 3.0;
    y *= 3.0;
    z *= 3.0;

    int size = (int)pow(_noise_pool_size, 0.33333333333333333);

    int xbase = (int)floor(x);
    int ybase = (int)floor(y);
    int zbase = (int)floor(z);

    double xinternal = x - (double)xbase;
    double yinternal = y - (double)ybase;
    double zinternal = z - (double)zbase;

    int x0 = (xbase - 1) % size;
    if (x0 < 0)
    {
        x0 += size;
    }
    int x1 = xbase % size;
    if (x1 < 0)
    {
        x1 += size;
    }
    int x2 = (xbase + 1) % size;
    if (x2 < 0)
    {
        x2 += size;
    }
    int x3 = (xbase + 2) % size;
    if (x3 < 0)
    {
        x3 += size;
    }

    int y0 = (ybase - 1) % size;
    if (y0 < 0)
    {
        y0 += size;
    }
    int y1 = ybase % size;
    if (y1 < 0)
    {
        y1 += size;
    }
    int y2 = (ybase + 1) % size;
    if (y2 < 0)
    {
        y2 += size;
    }
    int y3 = (ybase + 2) % size;
    if (y3 < 0)
    {
        y3 += size;
    }

    int z0 = (zbase - 1) % size;
    if (z0 < 0)
    {
        z0 += size;
    }
    int z1 = zbase % size;
    if (z1 < 0)
    {
        z1 += size;
    }
    int z2 = (zbase + 1) % size;
    if (z2 < 0)
    {
        z2 += size;
    }
    int z3 = (zbase + 2) % size;
    if (z3 < 0)
    {
        z3 += size;
    }

    double buf_cubic_x[4];
    double buf_cubic_y[4];
    double buf_cubic_z[4];

    buf_cubic_x[0] = _noise_pool[(y0 * size * size + x0 * size + z0) % _noise_pool_size];
    buf_cubic_x[1] = _noise_pool[(y0 * size * size + x1 * size + z0) % _noise_pool_size];
    buf_cubic_x[2] = _noise_pool[(y0 * size * size + x2 * size + z0) % _noise_pool_size];
    buf_cubic_x[3] = _noise_pool[(y0 * size * size + x3 * size + z0) % _noise_pool_size];
    buf_cubic_y[0] = toolsCubicInterpolate(buf_cubic_x, xinternal);

    buf_cubic_x[0] = _noise_pool[(y1 * size * size + x0 * size + z0) % _noise_pool_size];
    buf_cubic_x[1] = _noise_pool[(y1 * size * size + x1 * size + z0) % _noise_pool_size];
    buf_cubic_x[2] = _noise_pool[(y1 * size * size + x2 * size + z0) % _noise_pool_size];
    buf_cubic_x[3] = _noise_pool[(y1 * size * size + x3 * size + z0) % _noise_pool_size];
    buf_cubic_y[1] = toolsCubicInterpolate(buf_cubic_x, xinternal);

    buf_cubic_x[0] = _noise_pool[(y2 * size * size + x0 * size + z0) % _noise_pool_size];
    buf_cubic_x[1] = _noise_pool[(y2 * size * size + x1 * size + z0) % _noise_pool_size];
    buf_cubic_x[2] = _noise_pool[(y2 * size * size + x2 * size + z0) % _noise_pool_size];
    buf_cubic_x[3] = _noise_pool[(y2 * size * size + x3 * size + z0) % _noise_pool_size];
    buf_cubic_y[2] = toolsCubicInterpolate(buf_cubic_x, xinternal);

    buf_cubic_x[0] = _noise_pool[(y3 * size * size + x0 * size + z0) % _noise_pool_size];
    buf_cubic_x[1] = _noise_pool[(y3 * size * size + x1 * size + z0) % _noise_pool_size];
    buf_cubic_x[2] = _noise_pool[(y3 * size * size + x2 * size + z0) % _noise_pool_size];
    buf_cubic_x[3] = _noise_pool[(y3 * size * size + x3 * size + z0) % _noise_pool_size];
    buf_cubic_y[3] = toolsCubicInterpolate(buf_cubic_x, xinternal);

    buf_cubic_z[0] =  toolsCubicInterpolate(buf_cubic_y, yinternal);

    buf_cubic_x[0] = _noise_pool[(y0 * size * size + x0 * size + z1) % _noise_pool_size];
    buf_cubic_x[1] = _noise_pool[(y0 * size * size + x1 * size + z1) % _noise_pool_size];
    buf_cubic_x[2] = _noise_pool[(y0 * size * size + x2 * size + z1) % _noise_pool_size];
    buf_cubic_x[3] = _noise_pool[(y0 * size * size + x3 * size + z1) % _noise_pool_size];
    buf_cubic_y[0] = toolsCubicInterpolate(buf_cubic_x, xinternal);

    buf_cubic_x[0] = _noise_pool[(y1 * size * size + x0 * size + z1) % _noise_pool_size];
    buf_cubic_x[1] = _noise_pool[(y1 * size * size + x1 * size + z1) % _noise_pool_size];
    buf_cubic_x[2] = _noise_pool[(y1 * size * size + x2 * size + z1) % _noise_pool_size];
    buf_cubic_x[3] = _noise_pool[(y1 * size * size + x3 * size + z1) % _noise_pool_size];
    buf_cubic_y[1] = toolsCubicInterpolate(buf_cubic_x, xinternal);

    buf_cubic_x[0] = _noise_pool[(y2 * size * size + x0 * size + z1) % _noise_pool_size];
    buf_cubic_x[1] = _noise_pool[(y2 * size * size + x1 * size + z1) % _noise_pool_size];
    buf_cubic_x[2] = _noise_pool[(y2 * size * size + x2 * size + z1) % _noise_pool_size];
    buf_cubic_x[3] = _noise_pool[(y2 * size * size + x3 * size + z1) % _noise_pool_size];
    buf_cubic_y[2] = toolsCubicInterpolate(buf_cubic_x, xinternal);

    buf_cubic_x[0] = _noise_pool[(y3 * size * size + x0 * size + z1) % _noise_pool_size];
    buf_cubic_x[1] = _noise_pool[(y3 * size * size + x1 * size + z1) % _noise_pool_size];
    buf_cubic_x[2] = _noise_pool[(y3 * size * size + x2 * size + z1) % _noise_pool_size];
    buf_cubic_x[3] = _noise_pool[(y3 * size * size + x3 * size + z1) % _noise_pool_size];
    buf_cubic_y[3] = toolsCubicInterpolate(buf_cubic_x, xinternal);

    buf_cubic_z[1] =  toolsCubicInterpolate(buf_cubic_y, yinternal);

    buf_cubic_x[0] = _noise_pool[(y0 * size * size + x0 * size + z2) % _noise_pool_size];
    buf_cubic_x[1] = _noise_pool[(y0 * size * size + x1 * size + z2) % _noise_pool_size];
    buf_cubic_x[2] = _noise_pool[(y0 * size * size + x2 * size + z2) % _noise_pool_size];
    buf_cubic_x[3] = _noise_pool[(y0 * size * size + x3 * size + z2) % _noise_pool_size];
    buf_cubic_y[0] = toolsCubicInterpolate(buf_cubic_x, xinternal);

    buf_cubic_x[0] = _noise_pool[(y1 * size * size + x0 * size + z2) % _noise_pool_size];
    buf_cubic_x[1] = _noise_pool[(y1 * size * size + x1 * size + z2) % _noise_pool_size];
    buf_cubic_x[2] = _noise_pool[(y1 * size * size + x2 * size + z2) % _noise_pool_size];
    buf_cubic_x[3] = _noise_pool[(y1 * size * size + x3 * size + z2) % _noise_pool_size];
    buf_cubic_y[1] = toolsCubicInterpolate(buf_cubic_x, xinternal);

    buf_cubic_x[0] = _noise_pool[(y2 * size * size + x0 * size + z2) % _noise_pool_size];
    buf_cubic_x[1] = _noise_pool[(y2 * size * size + x1 * size + z2) % _noise_pool_size];
    buf_cubic_x[2] = _noise_pool[(y2 * size * size + x2 * size + z2) % _noise_pool_size];
    buf_cubic_x[3] = _noise_pool[(y2 * size * size + x3 * size + z2) % _noise_pool_size];
    buf_cubic_y[2] = toolsCubicInterpolate(buf_cubic_x, xinternal);

    buf_cubic_x[0] = _noise_pool[(y3 * size * size + x0 * size + z2) % _noise_pool_size];
    buf_cubic_x[1] = _noise_pool[(y3 * size * size + x1 * size + z2) % _noise_pool_size];
    buf_cubic_x[2] = _noise_pool[(y3 * size * size + x2 * size + z2) % _noise_pool_size];
    buf_cubic_x[3] = _noise_pool[(y3 * size * size + x3 * size + z2) % _noise_pool_size];
    buf_cubic_y[3] = toolsCubicInterpolate(buf_cubic_x, xinternal);

    buf_cubic_z[2] =  toolsCubicInterpolate(buf_cubic_y, yinternal);

    buf_cubic_x[0] = _noise_pool[(y0 * size * size + x0 * size + z3) % _noise_pool_size];
    buf_cubic_x[1] = _noise_pool[(y0 * size * size + x1 * size + z3) % _noise_pool_size];
    buf_cubic_x[2] = _noise_pool[(y0 * size * size + x2 * size + z3) % _noise_pool_size];
    buf_cubic_x[3] = _noise_pool[(y0 * size * size + x3 * size + z3) % _noise_pool_size];
    buf_cubic_y[0] = toolsCubicInterpolate(buf_cubic_x, xinternal);

    buf_cubic_x[0] = _noise_pool[(y1 * size * size + x0 * size + z3) % _noise_pool_size];
    buf_cubic_x[1] = _noise_pool[(y1 * size * size + x1 * size + z3) % _noise_pool_size];
    buf_cubic_x[2] = _noise_pool[(y1 * size * size + x2 * size + z3) % _noise_pool_size];
    buf_cubic_x[3] = _noise_pool[(y1 * size * size + x3 * size + z3) % _noise_pool_size];
    buf_cubic_y[1] = toolsCubicInterpolate(buf_cubic_x, xinternal);

    buf_cubic_x[0] = _noise_pool[(y2 * size * size + x0 * size + z3) % _noise_pool_size];
    buf_cubic_x[1] = _noise_pool[(y2 * size * size + x1 * size + z3) % _noise_pool_size];
    buf_cubic_x[2] = _noise_pool[(y2 * size * size + x2 * size + z3) % _noise_pool_size];
    buf_cubic_x[3] = _noise_pool[(y2 * size * size + x3 * size + z3) % _noise_pool_size];
    buf_cubic_y[2] = toolsCubicInterpolate(buf_cubic_x, xinternal);

    buf_cubic_x[0] = _noise_pool[(y3 * size * size + x0 * size + z3) % _noise_pool_size];
    buf_cubic_x[1] = _noise_pool[(y3 * size * size + x1 * size + z3) % _noise_pool_size];
    buf_cubic_x[2] = _noise_pool[(y3 * size * size + x2 * size + z3) % _noise_pool_size];
    buf_cubic_x[3] = _noise_pool[(y3 * size * size + x3 * size + z3) % _noise_pool_size];
    buf_cubic_y[3] = toolsCubicInterpolate(buf_cubic_x, xinternal);

    buf_cubic_z[3] =  toolsCubicInterpolate(buf_cubic_y, yinternal);

    return toolsCubicInterpolate(buf_cubic_z, zinternal) * 0.794 + 0.5;
}

/*double noiseNaiveGet4DValue(double x, double y, double z, double w)
{

}*/
