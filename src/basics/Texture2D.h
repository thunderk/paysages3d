#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include "basics_global.h"

#include <string>

namespace paysages {
namespace basics {

class BASICSSHARED_EXPORT Texture2D {
  public:
    Texture2D(int xsize, int ysize);
    ~Texture2D();

    void getSize(int *xsize, int *ysize) const;
    void setPixel(int x, int y, Color col);
    Color getPixel(int x, int y) const;
    Color getNearest(double dx, double dy) const;
    Color getLinear(double dx, double dy) const;
    Color getCubic(double dx, double dy) const;
    void fill(Color col);
    void add(Texture2D *other);
    void save(PackStream *stream) const;
    void load(PackStream *stream);
    void saveToFile(const string &filepath) const;

  private:
    int xsize;
    int ysize;
    Color *data;
};
}
}

#endif // TEXTURE2D_H
