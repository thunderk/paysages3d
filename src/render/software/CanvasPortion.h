#ifndef CANVASPORTION_H
#define CANVASPORTION_H

#include "software_global.h"

namespace paysages {
namespace software {

typedef struct {
    double red;
    double green;
    double blue;
} CanvasPreviewPixel;

/**
 * @brief Rectangular portion of a Canvas.
 *
 * Contains the pixels of a canvas region (CanvasPixel).
 */
class SOFTWARESHARED_EXPORT CanvasPortion
{
public:
    CanvasPortion();

    inline int getWidth() const {return width;}
    inline int getHeight() const {return height;}

    void setSize(int width, int height);

private:
    int width;
    int height;
    std::vector<CanvasPixel> *pixels;

    int preview_width;
    int preview_height;
    std::vector<CanvasPreviewPixel> *preview_pixels;
};

}
}

#endif // CANVASPORTION_H
