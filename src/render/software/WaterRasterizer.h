#ifndef WATERRASTERIZER_H
#define WATERRASTERIZER_H

#include "software_global.h"

#include "Rasterizer.h"

namespace paysages {
namespace software {

class WaterRasterizer : public Rasterizer {
  public:
    WaterRasterizer(SoftwareRenderer *renderer, RenderProgress *progress, unsigned short client_id);

    void rasterizeQuad(CanvasPortion *canvas, double x, double z, double size);

    virtual int prepareRasterization() override;
    virtual void rasterizeToCanvas(CanvasPortion *canvas) override;
    virtual Color shadeFragment(const CanvasFragment &fragment, const CanvasFragment *previous) const override;

    virtual void setQuality(double factor) override;

  private:
    int performTessellation(CanvasPortion *canvas);

  private:
    // Quality control
    double base_chunk_size;
};
}
}

#endif // WATERRASTERIZER_H
