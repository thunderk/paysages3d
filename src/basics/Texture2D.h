#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include "basics_global.h"

namespace paysages {
namespace basics {

class Texture2D
{
public:
    Texture2D(int xsize, int ysize);
    ~Texture2D();

    void getSize(int* xsize, int* ysize);
    void setPixel(int x, int y, Color col);
    Color getPixel(int x, int y);
    Color getNearest(double dx, double dy);
    Color getLinear(double dx, double dy);
    Color getCubic(double dx, double dy);
    void fill(Color col);
    void add(Texture2D* other);
    void save(PackStream* stream);
    void load(PackStream* stream);
    void saveToFile(const std::string &filepath);

private:
    int xsize;
    int ysize;
    Color* data;
};

}
}

#endif // TEXTURE2D_H
