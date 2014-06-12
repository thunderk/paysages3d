#ifndef CANVASPORTION_H
#define CANVASPORTION_H

#include "software_global.h"

namespace paysages {
namespace software {

/**
 * @brief Rectangular portion of a Canvas.
 *
 * Contains the pixels of a canvas region (CanvasPixel).
 */
class SOFTWARESHARED_EXPORT CanvasPortion
{
public:
    CanvasPortion(CanvasPreview* preview=NULL);
    ~CanvasPortion();

    inline int getWidth() const {return width;}
    inline int getHeight() const {return height;}
    int getFragmentCount(int x, int y) const;
    const CanvasFragment *getFrontFragment(int x, int y) const;

    void clear();
    void setSize(int width, int height);

    /**
     * @brief Add a fragment to the pixel located at (x, y).
     *
     * Checking x and y coordinates to be in the canvas portion should be done before this call.
     */
    void pushFragment(int x, int y, const CanvasFragment &fragment);

private:
    int width;
    int height;
    CanvasPixel *pixels;
    CanvasPreview* preview;
};

}
}

#endif // CANVASPORTION_H
