#ifndef SOFTWARECANVASRENDERER_H
#define SOFTWARECANVASRENDERER_H

#include "software_global.h"

#include "SoftwareRenderer.h"

namespace paysages {
namespace software {

/**
 * @brief Software rendering inside a Canvas surface.
 *
 * This class launches the rasterization process into canvas portions and
 * redirects post processing to the software renderer.
 *
 * It tries to keep a canvas portion rasterized ahead of the post processing.
 */
class SOFTWARESHARED_EXPORT SoftwareCanvasRenderer: public SoftwareRenderer
{
public:
    SoftwareCanvasRenderer();
    virtual ~SoftwareCanvasRenderer();

    inline const Canvas *getCanvas() const {return canvas;}

    /**
     * @brief Set the rendering size in pixels.
     *
     * Set 'samples' to something bigger than 1 to allow for the multi-sampling of pixels.
     */
    void setSize(int width, int height, int samples=1);

    /**
     * @brief Start the two-pass render process.
     */
    void render();

    /*!
     * \brief Get the list of objects that can be rasterized to polygons on a canvas.
     */
    virtual const std::vector<Rasterizer*> &getRasterizers() const;

protected:
    /**
     * @brief Rasterize the scenery into a canvas portion.
     *
     * If 'threaded' is true, the rasterization will take advantage of multiple CPU cores.
     */
    void rasterize(CanvasPortion* portion, bool threaded);

    /**
     * @brief Apply post-processing to fragments stored in the CanvasPortion.
     *
     * If 'threaded' is true, the post-processing will take advantage of multiple CPU cores.
     */
    void postProcess(CanvasPortion* portion, bool threaded=true);

private:
    Canvas* canvas;
    std::vector<Rasterizer*> rasterizers;
    bool started;
};

}
}

#endif // SOFTWARECANVASRENDERER_H
