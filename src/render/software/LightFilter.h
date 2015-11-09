#ifndef LIGHTFILTER_H
#define LIGHTFILTER_H

#include "software_global.h"

namespace paysages {
namespace software {

/**
 * @brief Interface for rendering parts that can alter light.
 */
class SOFTWARESHARED_EXPORT LightFilter {
  public:
    /**
     * @brief Apply filtering on a light component.
     *
     * This will alter the component and return if the component is still
     * useful.
     */
    virtual bool applyLightFilter(LightComponent &light, const Vector3 &at) = 0;
};
}
}

#endif // LIGHTFILTER_H
