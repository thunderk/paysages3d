#ifndef SKYRASTERIZER_H
#define SKYRASTERIZER_H

#include "software_global.h"

#include "Rasterizer.h"

namespace paysages {
namespace software {

class SOFTWARESHARED_EXPORT SkyRasterizer: public Rasterizer
{
public:
    SkyRasterizer(SoftwareRenderer* renderer, int client_id);

    virtual void rasterizeToCanvas(CanvasPortion* canvas);
    virtual Color shadeFragment(const CanvasFragment &fragment) const;
};

}
}

#endif // SKYRASTERIZER_H
