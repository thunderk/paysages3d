#ifndef OVERLAYRASTERIZER_H
#define OVERLAYRASTERIZER_H

#include "software_global.h"

#include "Rasterizer.h"

namespace paysages {
namespace software {

/**
 * Base class for overlay rasterizer.
 *
 * It's a rasterizer that puts a single quad in front of camera, in order to apply a shader on each pixel.
 */
class SOFTWARESHARED_EXPORT OverlayRasterizer: public Rasterizer
{
public:
    OverlayRasterizer(SoftwareRenderer *renderer, RenderProgress *progress);

    /**
     * Abstract method to implement to shade each pixel.
     */
    virtual Color processPixel(int x, int y, double relx, double rely) const = 0;

private:
    virtual int prepareRasterization();
    virtual void rasterizeToCanvas(CanvasPortion *canvas);
    virtual Color shadeFragment(const CanvasFragment &fragment, const CanvasFragment *previous) const;
};

}
}

#endif // OVERLAYRASTERIZER_H
