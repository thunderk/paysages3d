#ifndef TEXTURE3D_H
#define TEXTURE3D_H

#include "basics_global.h"

namespace paysages {
namespace basics {

class Texture3D
{
public:
    Texture3D(int xsize, int ysize, int zsize);
    ~Texture3D();

    void getSize(int* xsize, int* ysize, int* zsize);
    void setPixel(int x, int y, int z, Color col);
    Color getPixel(int x, int y, int z);
    Color getNearest(double dx, double dy, double dz);
    Color getLinear(double dx, double dy, double dz);
    Color getCubic(double dx, double dy, double dz);
    void fill(Color col);
    void add(Texture3D* other);
    void save(PackStream* stream);
    void load(PackStream* stream);
    void saveToFile(const std::string &filepath);

private:
    int xsize;
    int ysize;
    int zsize;
    Color* data;
};

}
}

#endif // TEXTURE3D_H
