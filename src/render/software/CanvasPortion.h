#ifndef CANVASPORTION_H
#define CANVASPORTION_H

#include "software_global.h"

namespace paysages {
namespace software {

/**
 * Rectangular portion of a Canvas.
 *
 * Contains the pixels of a canvas region (CanvasPixel).
 *
 * Pixels are not allocated until preparePixels is called.
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
     * Prepare (allocate in memory) the pixels area.
     */
    void preparePixels();

    /**
     * Add a fragment to the pixel located at (x, y).
     *
     * Checking x and y coordinates to be in the canvas portion should be done before this call.
     */
    void pushFragment(int x, int y, const CanvasFragment &fragment);

    /**
     * Get the CanvasPixel at a given coordinates.
     *
     * Checking x and y coordinates to be in the canvas portion should be done before this call.
     */
    const CanvasPixel &at(int x, int y);

    /**
     * Change the final color of the pixel.
     *
     * Checking x and y coordinates to be in the canvas portion should be done before this call.
     */
    void setColor(int x, int y, const Color &color);

private:
    int width;
    int height;
    CanvasPixel *pixels;
    CanvasPreview* preview;
};

}
}

#endif // CANVASPORTION_H
