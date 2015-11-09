#ifndef VEGETATIONRENDERER_H
#define VEGETATIONRENDERER_H

#include "software_global.h"

#include "LightFilter.h"

namespace paysages {
namespace software {

class SOFTWARESHARED_EXPORT VegetationRenderer : public LightFilter {
  public:
    VegetationRenderer(SoftwareRenderer *parent);

    /**
     * Totally enable or disable the vegetation layers rendering.
     */
    void setEnabled(bool enabled);

    inline SoftwareRenderer *getParent() const {
        return parent;
    }

    /**
     * Perform ray casting on a single instance.
     *
     * If *only_hit* is true, only care about hitting or not, do not compute the color.
     *
     * If *displaced* is true, *instance* is considered on already displaced terrain, else, terrain displacement is
     *applied.
     */
    RayCastingResult renderInstance(const SpaceSegment &segment, const VegetationInstance &instance,
                                    bool only_hit = false, bool displaced = false);

    /**
     * Perform ray casting on a given segment.
     */
    RayCastingResult getResult(const SpaceSegment &segment, bool only_hit = false);

    /**
     * Perform ray casting on a squared region.
     */
    RayCastingResult getBoundResult(const SpaceSegment &segment, double x, double z, bool only_hit = false,
                                    double xsize = 1.0, double zsize = 1.0);

    virtual bool applyLightFilter(LightComponent &light, const Vector3 &at) override;

  private:
    SoftwareRenderer *parent;
    bool enabled;
};
}
}

#endif // VEGETATIONRENDERER_H
