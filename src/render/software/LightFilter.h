#pragma once

#include "software_global.h"

namespace paysages {
namespace software {

/**
 * Interface for rendering parts that can alter light.
 */
class SOFTWARESHARED_EXPORT LightFilter {
  public:
    /**
     * Apply filtering on a light component.
     *
     * This will alter the component and return if the component is still
     * useful.
     */
    virtual bool applyLightFilter(LightComponent &light, const Vector3 &at) = 0;
};
}
}
