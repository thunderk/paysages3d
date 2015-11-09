#ifndef VEGETATIONRASTERIZER_H
#define VEGETATIONRASTERIZER_H

#include "software_global.h"

#include "TerrainRasterizer.h"

namespace paysages {
namespace software {

class SOFTWARESHARED_EXPORT VegetationRasterizer : public TerrainRasterizer {
  public:
    VegetationRasterizer(SoftwareRenderer *renderer, RenderProgress *progress, int client_id);

    virtual Color shadeFragment(const CanvasFragment &fragment, const CanvasFragment *previous) const override;
};
}
}

#endif // VEGETATIONRASTERIZER_H
