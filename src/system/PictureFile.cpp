#include "PictureFile.h"

#include <QImage>
#include <QColor>

PictureFile::PictureFile()
{
}

// Transitional C-API

int systemSavePictureFile(const char* filepath, PictureCallbackSavePixel callback_pixel, void* data, int width, int height)
{
    QImage result(width, height, QImage::Format_ARGB32);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            result.setPixel(x, height - 1 - y, callback_pixel(data, x, y));
        }
    }

    return result.save(filepath);
}
