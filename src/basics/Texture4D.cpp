#include "Texture4D.h"

#include <cassert>
#include <cmath>
#include "Color.h"
#include "PackStream.h"
#include "PictureWriter.h"

Texture4D::Texture4D(int xsize, int ysize, int zsize, int wsize) {
    assert(xsize > 0 && ysize > 0 && zsize > 0 && wsize > 0);

    this->xsize = xsize;
    this->ysize = ysize;
    this->zsize = zsize;
    this->wsize = wsize;
    this->data = new Color[xsize * ysize * zsize * wsize];
}

Texture4D::~Texture4D() {
    delete[] data;
}

void Texture4D::getSize(int *xsize, int *ysize, int *zsize, int *wsize) const {
    *xsize = this->xsize;
    *ysize = this->ysize;
    *zsize = this->zsize;
    *wsize = this->wsize;
}

void Texture4D::setPixel(int x, int y, int z, int w, Color col) {
    assert(x >= 0 && x < this->xsize);
    assert(y >= 0 && y < this->ysize);
    assert(z >= 0 && z < this->zsize);
    assert(w >= 0 && w < this->wsize);

    this->data[w * this->xsize * this->ysize * this->zsize + z * this->xsize * this->ysize + y * this->xsize + x] = col;
}

Color Texture4D::getPixel(int x, int y, int z, int w) const {
    assert(x >= 0 && x < this->xsize);
    assert(y >= 0 && y < this->ysize);
    assert(z >= 0 && z < this->zsize);
    assert(w >= 0 && w < this->wsize);

    return this
        ->data[w * this->xsize * this->ysize * this->zsize + z * this->xsize * this->ysize + y * this->xsize + x];
}

Color Texture4D::getNearest(double dx, double dy, double dz, double dw) const {
    if (dx < 0.0)
        dx = 0.0;
    if (dx > 1.0)
        dx = 1.0;
    if (dy < 0.0)
        dy = 0.0;
    if (dy > 1.0)
        dy = 1.0;
    if (dz < 0.0)
        dz = 0.0;
    if (dz > 1.0)
        dz = 1.0;
    if (dw < 0.0)
        dw = 0.0;
    if (dw > 1.0)
        dw = 1.0;

    int ix = (int)(dx * (double)(this->xsize - 1));
    int iy = (int)(dy * (double)(this->ysize - 1));
    int iz = (int)(dz * (double)(this->zsize - 1));
    int iw = (int)(dw * (double)(this->wsize - 1));

    assert(ix >= 0 && ix < this->xsize);
    assert(iy >= 0 && iy < this->ysize);
    assert(iz >= 0 && iz < this->zsize);
    assert(iw >= 0 && iw < this->wsize);

    return this
        ->data[iw * this->xsize * this->ysize * this->zsize + iz * this->xsize * this->ysize + iy * this->xsize + ix];
}

Color Texture4D::getLinear(double dx, double dy, double dz, double dw) const {
    if (dx < 0.0)
        dx = 0.0;
    if (dx > 1.0)
        dx = 1.0;
    if (dy < 0.0)
        dy = 0.0;
    if (dy > 1.0)
        dy = 1.0;
    if (dz < 0.0)
        dz = 0.0;
    if (dz > 1.0)
        dz = 1.0;
    if (dw < 0.0)
        dw = 0.0;
    if (dw > 1.0)
        dw = 1.0;

    dx *= (double)(this->xsize - 1);
    dy *= (double)(this->ysize - 1);
    dz *= (double)(this->zsize - 1);
    dw *= (double)(this->wsize - 1);

    int ix = (int)floor(dx);
    if (ix == this->xsize - 1) {
        ix--;
    }
    int iy = (int)floor(dy);
    if (iy == this->ysize - 1) {
        iy--;
    }
    int iz = (int)floor(dz);
    if (iz == this->zsize - 1) {
        iz--;
    }
    int iw = (int)floor(dw);
    if (iw == this->wsize - 1) {
        iw--;
    }

    dx -= (double)ix;
    dy -= (double)iy;
    dz -= (double)iz;
    dw -= (double)iw;

    Color *data = this->data + iw * this->xsize * this->ysize * this->zsize + iz * this->xsize * this->ysize +
                  iy * this->xsize + ix;
    Color cz1, cz2;

    Color cx1 = data->lerp(*(data + 1), dx);
    Color cx2 = (data + this->xsize)->lerp(*(data + this->xsize + 1), dx);
    Color cy1 = cx1.lerp(cx2, dy);

    data += this->xsize * this->ysize;
    cx1 = data->lerp(*(data + 1), dx);
    cx2 = (data + this->xsize)->lerp(*(data + this->xsize + 1), dx);
    Color cy2 = cx1.lerp(cx2, dy);

    data += this->xsize * this->ysize * this->zsize;
    cz1 = cy1.lerp(cy2, dz);

    cx1 = data->lerp(*(data + 1), dx);
    cx2 = (data + this->xsize)->lerp(*(data + this->xsize + 1), dx);
    cy1 = cx1.lerp(cx2, dy);

    cx1 = data->lerp(*(data + 1), dx);
    cx2 = (data + this->xsize)->lerp(*(data + this->xsize + 1), dx);
    cy2 = cx1.lerp(cx2, dy);

    cz2 = cy1.lerp(cy2, dz);

    return cz1.lerp(cz2, dw);
}

Color Texture4D::getCubic(double dx, double dy, double dz, double dw) const {
    /* TODO */
    return getLinear(dx, dy, dz, dw);
}

void Texture4D::fill(Color col) {
    int i, n;
    n = this->xsize * this->ysize * this->zsize * this->wsize;
    for (i = 0; i < n; i++) {
        this->data[i] = col;
    }
}

void Texture4D::add(Texture4D *source) {
    int i, n;

    assert(source->xsize == this->xsize);
    assert(source->ysize == this->ysize);
    assert(source->zsize == this->zsize);
    assert(source->wsize == this->wsize);

    n = source->xsize * source->ysize * source->zsize * source->wsize;
    for (i = 0; i < n; i++) {
        this->data[i].r += source->data[i].r;
        this->data[i].g += source->data[i].g;
        this->data[i].b += source->data[i].b;
        /* destination->data[i].a += source->data[i].a; */
    }
}

void Texture4D::save(PackStream *stream) const {
    int i, n;
    stream->write(&this->xsize);
    stream->write(&this->ysize);
    stream->write(&this->zsize);
    stream->write(&this->wsize);
    n = this->xsize * this->ysize * this->zsize * this->wsize;
    for (i = 0; i < n; i++) {
        (this->data + i)->save(stream);
    }
}

void Texture4D::load(PackStream *stream) {
    int i, n;
    stream->read(&this->xsize);
    stream->read(&this->ysize);
    stream->read(&this->zsize);
    stream->read(&this->wsize);
    n = this->xsize * this->ysize * this->zsize * this->wsize;
    delete[] this->data;
    this->data = new Color[n];
    for (i = 0; i < n; i++) {
        (this->data + i)->load(stream);
    }
}

class Texture4DWriter : public PictureWriter {
  public:
    Texture4DWriter(const Texture4D *tex) : tex(tex) {
    }

    virtual unsigned int getPixel(int x, int y) override {
        int xsize, ysize, zsize, wsize;
        tex->getSize(&xsize, &ysize, &zsize, &wsize);

        int w = x / xsize;
        x = x % xsize;

        int z = y / ysize;
        y = y % ysize;

        return tex->getPixel(x, y, z, w).to32BitBGRA();
    }

  private:
    const Texture4D *tex;
};

void Texture4D::saveToFile(const string &filepath) const {
    Texture4DWriter writer(this);
    writer.save(filepath, xsize * wsize, ysize * zsize);
}
