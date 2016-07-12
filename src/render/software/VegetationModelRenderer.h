#pragma once

#include "software_global.h"

#include "LightFilter.h"

namespace paysages {
namespace software {

/**
 * Renderer of a single instance of vegetation model (e.g a single tree).
 */
class SOFTWARESHARED_EXPORT VegetationModelRenderer : public LightFilter {
  public:
    VegetationModelRenderer(SoftwareRenderer *parent, const VegetationModelDefinition *model);
    virtual ~VegetationModelRenderer();

    /**
     * Get the final color of this model on a segment.
     *
     * Coordinates should be expressed as relative to the model origin point.
     *
     * If only_hit is True, we only look whether there is a hit or not.
     */
    VegetationResult getResult(const SpaceSegment &segment, bool only_hit = false) const;

    /**
     * Internal (relative) light filter.
     *
     * The 'at' parameter should be expressed as relative to the model origin point.
     */
    virtual bool applyLightFilter(LightComponent &light, const Vector3 &at);

  private:
    SoftwareRenderer *parent;
    const VegetationModelDefinition *model;
};
}
}
