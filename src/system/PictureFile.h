#ifndef PICTUREFILE_H
#define PICTUREFILE_H

#include "system_global.h"

#ifdef __cplusplus

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

extern "C" {
#endif

// Transitional C-API

#ifndef TYPEDEF_COLOR
typedef struct
{
    double r;
    double g;
    double b;
    double a;
} Color;
#endif

typedef Color (*PictureCallbackSavePixel)(void* data, int x, int y);
int systemSavePictureFile(const char* filepath, PictureCallbackSavePixel callback_pixel, void* data, int width, int height);

#ifdef __cplusplus
}
#endif

#endif // PICTUREFILE_H
