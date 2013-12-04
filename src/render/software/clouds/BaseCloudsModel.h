#ifndef BASECLOUDSMODEL_H
#define BASECLOUDSMODEL_H

#include "../software_global.h"

#include "Color.h"

namespace paysages {
namespace software {

/*!
 * \brief Abstract class for all cloud models (cirrus, cumulus...).
 */
class BaseCloudsModel
{
public:
    BaseCloudsModel(CloudLayerDefinition *layer);
    virtual ~BaseCloudsModel();

    virtual void update();

    virtual void getAltitudeRange(double *min_altitude, double *max_altitude) const;
    virtual void getDetailRange(double *min_step, double *max_step) const;
    virtual double getProbability(const Vector3 &location, double radius) const;
    virtual double getDensity(const Vector3 &location) const;
    virtual Color filterLight(const Color &light, double length, double density) const;
    virtual Color applyLightExit(const Color &light, const Vector3 &light_direction, const Vector3 &direction_to_eye) const;

    inline CloudLayerDefinition* getLayer() const {return layer;}

protected:
    CloudLayerDefinition *layer;
};

}
}

#endif // BASECLOUDSMODEL_H
