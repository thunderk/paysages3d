#pragma once

#include "software_global.h"

namespace paysages {
namespace software {

/**
 * 3D sampler for "god rays".
 */
class SOFTWARESHARED_EXPORT GodRaysSampler {
  public:
    GodRaysSampler();
    ~GodRaysSampler();

    inline const SpaceSegment &getBounds() const {
        return *bounds;
    }
    inline double getSamplingStep() const {
        return sampling_step;
    }
    inline double getMaxLength() const {
        return max_length;
    }
    inline double getWalkStep() const {
        return walk_step;
    }

    /**
     * Prepare the sampler from a renderer.
     */
    void prepare(SoftwareRenderer *renderer);

    /**
     * Reset the sampler cache.
     *
     * This needs to be called after all setXXX method have been called.
     */
    void reset();

    /**
     * Enable or disable the god rays effect.
     */
    void setEnabled(bool enabled);

    /**
     * Set the lighting manager to use for raw sampling.
     */
    void setLighting(LightingManager *manager);

    /**
     * Set the overall quality factor (0.0-1.0).
     */
    void setQuality(double factor);

    /**
     * Set the quality indicators.
     */
    void setQuality(double sampling_step, double max_length, double walk_step);

    /**
     * Set the camera location.
     *
     * This will fix the limits of sampling data, around the camera location.
     */
    void setCameraLocation(const Vector3 &location);

    /**
     * Set the altitude boundaries.
     */
    void setAltitudes(double low, double high);

    /**
     * Get the number of samples in each dimension.
     */
    void getSamples(int *x, int *y, int *z) const;

    /**
     * Get the raw light at a location (without using the cached sampling).
     */
    Color getRawLight(const Vector3 &location, bool filtered) const;

    /**
     * Get the lighting factor at a given point, using (and filling) the cached sampling.
     */
    double getCachedLight(const Vector3 &location);

    /**
     * Get the god rays effect on a space segment.
     */
    GodRaysResult getResult(const SpaceSegment &segment);

    /**
     * Apply the god rays effect to a location.
     *
     * 'raw' is the shaded color, without atmospheric effects.
     * 'atmosphered' is the shaded color, with atmospheric effects applied.
     */
    Color apply(const Color &raw, const Color &atmosphered, const Vector3 &location);

  private:
    double getCache(int x, int y, int z);

  private:
    bool enabled;
    SpaceSegment *bounds;

    GodRaysDefinition *definition;

    double sampling_step;
    double max_length;
    double walk_step;

    int samples_x;
    int samples_y;
    int samples_z;

    double low_altitude;
    double high_altitude;
    Vector3 *camera_location;

    LightingManager *lighting;

    double *data;
};
}
}
