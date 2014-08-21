#ifndef CLOUDMODELALTOCUMULUS_H
#define CLOUDMODELALTOCUMULUS_H

#include "../software_global.h"

#include "BaseCloudsModel.h"

namespace paysages {
namespace software {

class CloudModelAltoCumulus : public BaseCloudsModel
{
public:
    CloudModelAltoCumulus(CloudLayerDefinition* layer);
    virtual ~CloudModelAltoCumulus();

    virtual void update() override;

    virtual void getAltitudeRange(double *min_altitude, double *max_altitude) const override;
    virtual double getDensity(const Vector3 &location) const override;

private:
    NoiseGenerator* noise;
};

}
}

#endif // CLOUDMODELALTOCUMULUS_H
