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
    CanvasPortion(int index=0, CanvasPreview *preview=NULL);
    virtual ~CanvasPortion();

    inline int getWidth() const {return width;}
    inline int getHeight() const {return height;}
    inline int getXOffset() const {return xoffset;}
    inline int getYOffset() const {return yoffset;}
    int getFragmentCount(int x, int y) const;
    const CanvasFragment *getFrontFragment(int x, int y) const;

    void clear();
    void setSize(int width, int height, int xoffset=0, int yoffset=0);

    /**
     * Prepare (allocate in memory) the pixels area.
     */
    void preparePixels();

    /**
     * Discard the memory used by pixels.
     *
     * If save is true, the portion will be saved to disk before.
     */
    void discardPixels(bool save=true);

    /**
     * Save the portion to a picture file on disk.
     */
    void saveToDisk();

    /**
     * Bind a stream to pixel data, and position it on a given pixel.
     *
     * Returns true if the stream was successfully located, false if it was not possible.
     */
    bool getReadStream(PackStream &stream, int x=0, int y=0);

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
    int index;
    int width;
    int height;
    int xoffset;
    int yoffset;
    CanvasPixel *pixels;
    CanvasPreview *preview;
    std::string filepath;
};

}
}

#endif // CANVASPORTION_H
