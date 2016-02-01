#ifndef CLOUDMODELSTRATOCUMULUS_H
#define CLOUDMODELSTRATOCUMULUS_H

#include "../software_global.h"

#include "BaseCloudsModel.h"

namespace paysages {
namespace software {

class CloudModelStratoCumulus : public BaseCloudsModel {
  public:
    CloudModelStratoCumulus(CloudLayerDefinition *layer);
    virtual ~CloudModelStratoCumulus();

    virtual void update() override;

    virtual void getAltitudeRange(double *min_altitude, double *max_altitude) const override;
    virtual double getDensity(const Vector3 &location, double precision) const override;

  private:
    NoiseGenerator *noise;
};
}
}

#endif // CLOUDMODELSTRATOCUMULUS_H
