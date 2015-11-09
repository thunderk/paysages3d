#ifndef CLOUDMODELCUMULONIMBUS_H
#define CLOUDMODELCUMULONIMBUS_H

#include "../software_global.h"

#include "BaseCloudsModel.h"

namespace paysages {
namespace software {

class CloudModelCumuloNimbus : public BaseCloudsModel {
  public:
    CloudModelCumuloNimbus(CloudLayerDefinition *layer);
    virtual ~CloudModelCumuloNimbus();

    virtual void update() override;

    virtual void getAltitudeRange(double *min_altitude, double *max_altitude) const override;
    virtual double getDensity(const Vector3 &location) const override;

  private:
    NoiseGenerator *noise;
};
}
}

#endif // CLOUDMODELCUMULONIMBUS_H
