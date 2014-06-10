#include "CanvasPortion.h"

#include <cassert>

#include "CanvasPixel.h"

#define CHECK_COORDINATES() assert(x >= 0); \
    assert(x < width); \
    assert(y >= 0); \
    assert(y < height)

CanvasPortion::CanvasPortion()
{
    width = 1;
    height = 1;
    pixels = new CanvasPixel[1];
}

CanvasPortion::~CanvasPortion()
{
    delete[] pixels;
}

int CanvasPortion::getFragmentCount(int x, int y) const
{
    CHECK_COORDINATES();

    return pixels[y * width + x].getFragmentCount();
}

const CanvasFragment *CanvasPortion::getFrontFragment(int x, int y) const
{
    CHECK_COORDINATES();

    return pixels[y * width + x].getFrontFragment();
}

void CanvasPortion::clear()
{
    int n = width * height;
    for (int i = 0; i < n; i++)
    {
        pixels[i].reset();
    }
}

void CanvasPortion::setSize(int width, int height)
{
    this->width = width;
    this->height = height;

    delete[] pixels;
    pixels = new CanvasPixel[width * height];
}

void CanvasPortion::pushFragment(int x, int y, const CanvasFragment &fragment)
{
    CHECK_COORDINATES();

    CanvasPixel &pixel = pixels[y * width + x];
    pixel.pushFragment(fragment);
}
