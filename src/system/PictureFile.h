#ifndef PICTUREFILE_H
#define PICTUREFILE_H

#include "system_global.h"

namespace paysages
{
namespace system
{

class SYSTEMSHARED_EXPORT PictureFile
{
public:
    PictureFile();
};

}
}

// Transitional C-API

// Les pixels doivent être fournis en RGBA
typedef unsigned int (*PictureCallbackSavePixel)(void* data, int x, int y);
SYSTEMSHARED_EXPORT int systemSavePictureFile(const char* filepath, PictureCallbackSavePixel callback_pixel, void* data, int width, int height);

#endif // PICTUREFILE_H
