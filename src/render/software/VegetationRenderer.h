#ifndef VEGETATIONRENDERER_H
#define VEGETATIONRENDERER_H

#include "software_global.h"

#include "LightFilter.h"

namespace paysages {
namespace software {

class SOFTWARESHARED_EXPORT VegetationRenderer: public LightFilter
{
public:
    VegetationRenderer(SoftwareRenderer *parent);

    /**
     * Totally enable or disable the vegetation layers rendering.
     */
    void setEnabled(bool enabled);

    inline SoftwareRenderer *getParent() const {return parent;}

    /**
     * Perform ray casting on a single instance.
     */
    RayCastingResult renderInstance(const SpaceSegment &segment, const VegetationInstance &instance, bool only_hit=false);

    /**
     * Perform ray casting on a given segment.
     */
    RayCastingResult getResult(const SpaceSegment &segment, bool only_hit=false);

    virtual bool applyLightFilter(LightComponent &light, const Vector3 &at) override;

private:
    SoftwareRenderer *parent;
    bool enabled;
};

}
}

#endif // VEGETATIONRENDERER_H
