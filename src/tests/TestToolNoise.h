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
        // The noise will yield its value at first iteration, then will collapse to 0
        setStep(0.0);
    }
    virtual double getBase3d(double, double, double) const override {
        return value;
    }

  private:
    double value;
};
}

#endif // TESTTOOLNOISE_H
