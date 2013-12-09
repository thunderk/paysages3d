#include "Texture3D.h"

#include <cassert>
#include "Color.h"
#include "PackStream.h"
#include "PictureWriter.h"

Texture3D::Texture3D(int xsize, int ysize, int zsize)
{
    assert(xsize > 0 && ysize > 0 && zsize > 0);

    this->xsize = xsize;
    this->ysize = ysize;
    this->zsize = zsize;
    this->data = new Color[xsize * ysize * zsize];
}

Texture3D::~Texture3D()
{
    delete[] data;
}

void Texture3D::getSize(int* xsize, int* ysize, int* zsize)
{
    *xsize = this->xsize;
    *ysize = this->ysize;
    *zsize = this->zsize;
}

void Texture3D::setPixel(int x, int y, int z, Color col)
{
    assert(x >= 0 && x < this->xsize);
    assert(y >= 0 && y < this->ysize);
    assert(z >= 0 && z < this->ysize);

    this->data[z * this->xsize * this->ysize + y * this->xsize + x] = col;
}

Color Texture3D::getPixel(int x, int y, int z)
{
    assert(x >= 0 && x < this->xsize);
    assert(y >= 0 && y < this->ysize);
    assert(z >= 0 && z < this->zsize);

    return this->data[z * this->xsize * this->ysize + y * this->xsize + x];
}

Color Texture3D::getNearest(double dx, double dy, double dz)
{
    if (dx < 0.0) dx = 0.0;
    if (dx > 1.0) dx = 1.0;
    if (dy < 0.0) dy = 0.0;
    if (dy > 1.0) dy = 1.0;
    if (dz < 0.0) dz = 0.0;
    if (dz > 1.0) dz = 1.0;

    int ix = (int)(dx * (double)(this->xsize - 1));
    int iy = (int)(dy * (double)(this->ysize - 1));
    int iz = (int)(dz * (double)(this->zsize - 1));

    assert(ix >= 0 && ix < this->xsize);
    assert(iy >= 0 && iy < this->ysize);
    assert(iz >= 0 && iz < this->zsize);

    return this->data[iz * this->xsize * this->ysize + iy * this->xsize + ix];
}

Color Texture3D::getLinear(double dx, double dy, double dz)
{
    if (dx < 0.0) dx = 0.0;
    if (dx > 1.0) dx = 1.0;
    if (dy < 0.0) dy = 0.0;
    if (dy > 1.0) dy = 1.0;
    if (dz < 0.0) dz = 0.0;
    if (dz > 1.0) dz = 1.0;

    dx *= (double)(this->xsize - 1);
    dy *= (double)(this->ysize - 1);
    dz *= (double)(this->zsize - 1);

    int ix = (int)floor(dx);
    if (ix == this->xsize - 1)
    {
        ix--;
    }
    int iy = (int)floor(dy);
    if (iy == this->ysize - 1)
    {
        iy--;
    }
    int iz = (int)floor(dz);
    if (iz == this->zsize - 1)
    {
        iz--;
    }

    dx -= (double)ix;
    dy -= (double)iy;
    dz -= (double)iz;

    Color* data = this->data + iz * this->xsize * this->ysize + iy * this->xsize + ix;

    Color cx1 = data->lerp(*(data + 1), dx);
    Color cx2 = (data + this->xsize)->lerp(*(data + this->xsize + 1), dx);
    Color cy1 = cx1.lerp(cx2, dy);

    data += this->xsize * this->ysize;
    cx1 = data->lerp(*(data + 1), dx);
    cx2 = (data + this->xsize)->lerp(*(data + this->xsize + 1), dx);
    Color cy2 = cx1.lerp(cx2, dy);

    return cy1.lerp(cy2, dz);
}

Color Texture3D::getCubic(double dx, double dy, double dz)
{
    /* TODO */
    return getLinear(dx, dy, dz);
}

void Texture3D::fill(Color col)
{
    int i, n;
    n = this->xsize * this->ysize * this->zsize;
    for (i = 0; i < n; i++)
    {
        this->data[i] = col;
    }
}

void Texture3D::add(Texture3D* source)
{
    int i, n;

    assert(source->xsize == this->xsize);
    assert(source->ysize == this->ysize);
    assert(source->zsize == this->zsize);

    n = source->xsize * source->ysize * source->zsize;
    for (i = 0; i < n; i++)
    {
        this->data[i].r += source->data[i].r;
        this->data[i].g += source->data[i].g;
        this->data[i].b += source->data[i].b;
        /* destination->data[i].a += source->data[i].a; */
    }
}

void Texture3D::save(PackStream* stream)
{
    int i, n;
    stream->write(&this->xsize);
    stream->write(&this->ysize);
    stream->write(&this->zsize);
    n = this->xsize * this->ysize * this->zsize;
    for (i = 0; i < n; i++)
    {
        colorSave(stream, this->data + i);
    }
}

void Texture3D::load(PackStream* stream)
{
    int i, n;
    stream->read(&this->xsize);
    stream->read(&this->ysize);
    stream->read(&this->zsize);
    n = this->xsize * this->ysize * this->zsize;
    delete[] this->data;
    this->data = new Color[n];
    for (i = 0; i < n; i++)
    {
        colorLoad(stream, this->data + i);
    }
}

class Texture3DWriter:public PictureWriter
{
public:
    Texture3DWriter(Texture3D *tex): tex(tex) {}

    virtual unsigned int getPixel(int x, int y) override
    {
        int xsize, ysize, zsize;
        tex->getSize(&xsize, &ysize, &zsize);

        int z = y / ysize;
        y = y % ysize;

        return tex->getPixel(x, y, z).to32BitBGRA();
    }
private:
    Texture3D *tex;
};

void Texture3D::saveToFile(const std::string &filepath)
{
    Texture3DWriter writer(this);
    writer.save(filepath, xsize, ysize * zsize);
}
