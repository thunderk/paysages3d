#ifndef TEXTURE4D_H
#define TEXTURE4D_H

#include "basics_global.h"

namespace paysages {
namespace basics {

class Texture4D
{
public:
    Texture4D(int xsize, int ysize, int zsize, int wsize);
    ~Texture4D();

    void getSize(int* xsize, int* ysize, int* zsize, int* wsize);
    void setPixel(int x, int y, int z, int w, Color col);
    Color getPixel(int x, int y, int z, int w);
    Color getNearest(double dx, double dy, double dz, double dw);
    Color getLinear(double dx, double dy, double dz, double dw);
    Color getCubic(double dx, double dy, double dz, double dw);
    void fill(Color col);
    void add(Texture4D* other);
    void save(PackStream* stream);
    void load(PackStream* stream);
    void saveToFile(const std::string &filepath);

private:
    int xsize;
    int ysize;
    int zsize;
    int wsize;
    Color* data;
};

}
}

#endif // TEXTURE4D_H
