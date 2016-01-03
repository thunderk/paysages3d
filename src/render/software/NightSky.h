#ifndef NIGHTSKY_H
#define NIGHTSKY_H

#include "software_global.h"

#include "LightSource.h"

namespace paysages {
namespace software {

/**
 * Night sky renderer.
 */
class SOFTWARESHARED_EXPORT NightSky : public LightSource {
  public:
    NightSky(SoftwareRenderer *renderer);
    virtual ~NightSky();

    /**
     * Update the night sky renderer, when the scenery or parent renderer changed.
     */
    void update();

    /**
     * Get the color of the night sky at a given direction.
     *
     * *altitude* is above water level, in coordinate units (not kilometers).
     */
    virtual const Color getColor(double altitude, const Vector3 &direction);

    virtual bool getLightsAt(vector<LightComponent> &result, const Vector3 &location) const override;

  private:
    SoftwareRenderer *renderer;
};
}
}

#endif // NIGHTSKY_H
