#pragma once

#include "software_global.h"

namespace paysages {
namespace software {

class BaseCloudLayerRenderer {
  public:
    BaseCloudLayerRenderer(SoftwareRenderer *parent);
    virtual ~BaseCloudLayerRenderer();

    virtual bool optimizeSearchLimits(BaseCloudsModel *model, Vector3 *start, Vector3 *end);

    virtual Color getColor(BaseCloudsModel *model, const Vector3 &eye, const Vector3 &location);
    virtual bool alterLight(BaseCloudsModel *model, LightComponent *light, const Vector3 &eye, const Vector3 &location);

    virtual void setQuality(double quality);

  protected:
    SoftwareRenderer *parent;
    double quality;
};
}
}
