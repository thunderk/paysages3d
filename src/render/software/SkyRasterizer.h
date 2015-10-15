#ifndef SKYRASTERIZER_H
#define SKYRASTERIZER_H

#include "software_global.h"

#include "Rasterizer.h"

namespace paysages {
namespace software {

class SOFTWARESHARED_EXPORT SkyRasterizer: public Rasterizer
{
public:
    SkyRasterizer(SoftwareRenderer* renderer, RenderProgress *progress, int client_id);

    virtual int prepareRasterization() override;
    virtual void rasterizeToCanvas(CanvasPortion* canvas) override;
    virtual Color shadeFragment(const CanvasFragment &fragment, const CanvasFragment *previous) const override;

    void setQuality(int res_i, int res_j);
    virtual void setQuality(double factor) override;

private:
    int res_i;
    int res_j;
};

}
}

#endif // SKYRASTERIZER_H
