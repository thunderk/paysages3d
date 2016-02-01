#ifndef CLOUDMODELCIRRUS_H
#define CLOUDMODELCIRRUS_H

#include "../software_global.h"

#include "BaseCloudsModel.h"

namespace paysages {
namespace software {

class CloudModelCirrus : public BaseCloudsModel {
  public:
    CloudModelCirrus(CloudLayerDefinition *layer);
    virtual ~CloudModelCirrus();

    virtual void update() override;

    virtual void getAltitudeRange(double *min_altitude, double *max_altitude) const override;
    virtual double getDensity(const Vector3 &location, double precision) const override;

  private:
    NoiseGenerator *noise;
};
}
}

#endif // CLOUDMODELCIRRUS_H
