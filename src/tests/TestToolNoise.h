#ifndef TESTTOOLNOISE_H
#define TESTTOOLNOISE_H

#include "FractalNoise.h"

namespace {
/**
 * Fractal noise that produces the same value anywhere.
 */
class ConstantFractalNoise : public FractalNoise {
  public:
    ConstantFractalNoise(double value) : value(value) {
        // The noise will yield its value at first iteration, then its height will collapse to 0
        setScaling(1.0, 0.0);
    }
    virtual double getBase3d(double, double, double) const {
        return value;
    }

  private:
    double value;
};
}

#endif // TESTTOOLNOISE_H
