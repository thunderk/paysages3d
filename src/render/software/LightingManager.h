#ifndef LIGHTINGMANAGER_H
#define LIGHTINGMANAGER_H

#include "software_global.h"

#include "LightComponent.h"

namespace paysages {
namespace software {

/**
 * @brief Global lighting manager.
 *
 * This manager handles the lights, light filters and final light rendering.
 *
 * There are both static and dynamic lights.
 * A dynamic light depends on the location at which the lighting occurs.
 */
class SOFTWARESHARED_EXPORT LightingManager
{
public:
    LightingManager();

    int getStaticLightsCount() const;
    int getSourcesCount() const;
    int getFiltersCount() const;

    /**
     * Clear the static lights.
     */
    void clearStaticLights();

    /**
     * Add a static light.
     */
    void addStaticLight(const LightComponent &light);

    /**
     * Register a source of dynamic lighting.
     */
    void registerSource(LightSource *source);

    /**
     * Remove a registered light source.
     */
    void unregisterSource(LightSource *source);

    /**
     * Register a filter that will receive all alterable lights.
     */
    void registerFilter(LightFilter *filter);

    /**
     * Remove a registered light filter.
     */
    void unregisterFilter(LightFilter *filter);

    /**
     * Alter the light component at a given location.
     *
     * Returns true if the light is useful
     */
    bool alterLight(LightComponent &component, const Vector3 &location);

    /**
     * Enable or disable the specularity lighting.
     */
    void setSpecularity(bool enabled);

    /**
     * Apply a final component on a surface material.
     */
    Color applyFinalComponent(const LightComponent &component, const Vector3 &eye, const Vector3 &location, const Vector3 &normal, const SurfaceMaterial &material);

    /**
     * Apply lighting to a surface at a given location.
     */
    Color apply(const Vector3 &eye, const Vector3 &location, const Vector3 &normal, const SurfaceMaterial &material);

private:
    int specularity;
    std::vector<LightComponent> static_lights;
    std::vector<LightFilter *> filters;
    std::vector<LightSource *> sources;

};

}
}

#endif // LIGHTINGMANAGER_H
