#include "CanvasPortion.h"

#include <cassert>

#include "CanvasPixel.h"
#include "CanvasPreview.h"

#define CHECK_COORDINATES() assert(x >= 0); \
    assert(x < width); \
    assert(y >= 0); \
    assert(y < height); \
    assert(pixels != NULL)

CanvasPortion::CanvasPortion(CanvasPreview* preview):
    preview(preview)
{
    width = 1;
    height = 1;
    pixels = NULL;
}

CanvasPortion::~CanvasPortion()
{
    if (pixels)
    {
        delete[] pixels;
    }
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
}

void CanvasPortion::preparePixels()
{
    if (pixels)
    {
        delete[] pixels;
    }
    pixels = new CanvasPixel[width * height];
    clear();
}

void CanvasPortion::pushFragment(int x, int y, const CanvasFragment &fragment)
{
    CHECK_COORDINATES();

    CanvasPixel &pixel = pixels[y * width + x];
    Color old_color = pixel.getComposite();

    pixel.pushFragment(fragment);

    if (preview)
    {
        preview->pushPixel(x, y, old_color, pixel.getComposite());
    }
}

const CanvasPixel &CanvasPortion::at(int x, int y)
{
    CHECK_COORDINATES();

    return pixels[y * width + x];
}

void CanvasPortion::setColor(int x, int y, const Color &color)
{
    CHECK_COORDINATES();

    CanvasPixel &pixel = pixels[y * width + x];
    Color old_color = pixel.getComposite();

    pixel.setComposite(color);

    if (preview)
    {
        preview->pushPixel(x, y, old_color, pixel.getComposite());
    }
}
