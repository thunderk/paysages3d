#pragma once

#include "basics_global.h"

#include "NoiseState.h"
#include <string>

namespace paysages {
namespace basics {

/**
 * Fractal noise generator, based on a sum of simple noise functions.
 */
class BASICSSHARED_EXPORT FractalNoise {
  public:
    FractalNoise();
    virtual ~FractalNoise();

    virtual void save(PackStream *stream) const;
    virtual void load(PackStream *stream);
    virtual void copy(FractalNoise *destination) const;

    inline double getScaling() const {
        return scaling;
    }
    inline double getHeight() const {
        return height;
    }
    inline double getStepScaling() const {
        return step_scaling;
    }
    inline double getStepHeight() const {
        return step_height;
    }

    /**
     * Randomize the noise (only the noise values, not its configuration).
     */
    void randomize(RandomGenerator &random);

    void setScaling(double scaling, double height = 1.0);
    void setStep(double scaling_factor, double height_factor = 1.0);
    void setState(const NoiseState &state);

    double get1d(double detail, double x) const;
    double get2d(double detail, double x, double y) const;
    double get3d(double detail, double x, double y, double z) const;

    /**
     * Get the triplanar noise value, according to a normal vector at a given location.
     *
     * Triplanar noise avoids stretching effects while applying a noise to texture a non-planar surface.
     */
    double getTriplanar(double detail, const Vector3 &location, const Vector3 &normal) const;

    /**
     * Estimate the range of values this generator will yield with a very small detail value.
     */
    void estimateRange(double *min, double *max, double detail = 0.000001) const;

    virtual double getBase1d(double x) const;
    virtual double getBase2d(double x, double y) const;
    /**
     * Base 3d noise function, returning (as much as possible) a value in the [0.5, 0.5] range.
     *
     * Other dimension noise (1d and 2d) can be provided, or this one will be used to simulate them.
     */
    virtual double getBase3d(double x, double y, double z) const = 0;

    /**
     * Test the noise distribution, and return the report.
     */
    string checkDistribution();

  private:
    NoiseState state;

    double scaling;
    double height;
    double step_scaling;
    double step_height;
};
}
}
