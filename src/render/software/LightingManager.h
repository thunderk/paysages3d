#ifndef LIGHTINGMANAGER_H
#define LIGHTINGMANAGER_H

#include "software_global.h"

namespace paysages {
namespace software {

/**
 * @brief Global lighting manager.
 *
 * This manager handles the light filters and final light rendering.
 */
class SOFTWARESHARED_EXPORT LightingManager
{
public:
    LightingManager();

    /**
     * @brief Register a filter that will receive all alterable lights.
     */
    void registerFilter(LightFilter* filter);

    /**
     * @brief Alter the light component at a given location.
     * @return true if the light is useful
     */
    bool alterLight(LightComponent &component, const Vector3 &location);

    /**
     * @brief Enable or disable the specularity lighting.
     */
    void setSpecularity(bool enabled);

    /**
     * @brief Apply a final component on a surface material.
     */
    Color applyFinalComponent(const LightComponent &component, const Vector3 &eye, const Vector3 &location, const Vector3 &normal, const SurfaceMaterial &material);

private:
    int specularity;

    std::vector<LightFilter*> filters;
};

}
}

#endif // LIGHTINGMANAGER_H
