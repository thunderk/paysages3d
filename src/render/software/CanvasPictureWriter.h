#ifndef CANVASPICTUREWRITER_H
#define CANVASPICTUREWRITER_H

#include "software_global.h"

#include "PictureWriter.h"

namespace paysages {
namespace software {

/**
 * Picture writer to create the final image from canvas portions.
 */
class SOFTWARESHARED_EXPORT CanvasPictureWriter: public PictureWriter
{
public:
    CanvasPictureWriter(const Canvas *canvas);
    virtual ~CanvasPictureWriter();

    /**
     * Set the antialias factor, 1 for no antialiasing.
     */
    void setAntialias(int antialias);

    /**
     * Set the color profile to apply to final pixels.
     */
    void setColorProfile(const ColorProfile &profile);

    /**
     * Start the saving process.
     *
     * Returns true if saving was successful.
     */
    bool saveCanvas(const std::string &filepath);

protected:
    virtual unsigned int getPixel(int x, int y) override;

private:
    Color getRawPixel(int x, int y);

private:
    const Canvas *canvas;
    int antialias;
    int width;
    int height;
    ColorProfile *profile;

    int cache_y;
    int cache_width;
    Color *cache;
};

}
}

#endif // CANVASPICTUREWRITER_H
