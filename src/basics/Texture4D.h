#pragma once

#include "basics_global.h"

#include <string>

namespace paysages {
namespace basics {

class BASICSSHARED_EXPORT Texture4D {
  public:
    Texture4D(int xsize, int ysize, int zsize, int wsize);
    ~Texture4D();

    void getSize(int *xsize, int *ysize, int *zsize, int *wsize) const;
    void setPixel(int x, int y, int z, int w, Color col);
    Color getPixel(int x, int y, int z, int w) const;
    Color getNearest(double dx, double dy, double dz, double dw) const;
    Color getLinear(double dx, double dy, double dz, double dw) const;
    Color getCubic(double dx, double dy, double dz, double dw) const;
    void fill(Color col);
    void add(Texture4D *other);
    void save(PackStream *stream) const;
    void load(PackStream *stream);
    void saveToFile(const string &filepath) const;

  private:
    int xsize;
    int ysize;
    int zsize;
    int wsize;
    Color *data;
};
}
}
