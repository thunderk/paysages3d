#include "CanvasPreview.h"

CanvasPreview::CanvasPreview()
{
    width = 1;
    height = 1;
    pixels = new CanvasPreviewPixel[1];
}

CanvasPreview::~CanvasPreview()
{
    delete [] pixels;
}

void CanvasPreview::setSize(int real_width, int real_height, int preview_width, int preview_height)
{
    delete [] pixels;
    pixels = new CanvasPreviewPixel[preview_width * preview_height];

    width = preview_width;
    height = preview_height;
}
