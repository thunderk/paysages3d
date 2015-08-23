#ifndef WATERRASTERIZER_H
#define WATERRASTERIZER_H

#include "software_global.h"

#include "Rasterizer.h"

namespace paysages {
namespace software {

class WaterRasterizer: public Rasterizer
{
public:
    WaterRasterizer(SoftwareRenderer* renderer, RenderProgress *progress, int client_id);

    void rasterizeQuad(CanvasPortion* canvas, double x, double z, double size);

    virtual int prepareRasterization() override;
    virtual void rasterizeToCanvas(CanvasPortion* canvas) override;
    virtual Color shadeFragment(const CanvasFragment &fragment) const override;

private:
    int performTessellation(CanvasPortion* canvas);
};

}
}

#endif // WATERRASTERIZER_H
