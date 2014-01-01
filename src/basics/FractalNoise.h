#ifndef FRACTALNOISE_H
#define FRACTALNOISE_H

#include "basics_global.h"

#include "NoiseState.h"

namespace paysages {
namespace basics {

/*!
 * \brief Fractal noise generator, based on a sum of simple noise functions.
 */
class BASICSSHARED_EXPORT FractalNoise
{
public:
    FractalNoise();

    void setScaling(double scaling, double height=1.0);
    void setStep(double scaling_factor, double height_factor=1.0);
    void setSlope(double slope_factor);
    void setRidge(double ridge_factor);

    double get1d(double detail, double x) const;
    double get2d(double detail, double x, double y) const;
    double get3d(double detail, double x, double y, double z) const;

    virtual double getBase1d(double x) const;
    virtual double getBase2d(double x, double y) const;
    virtual double getBase3d(double x, double y, double z) const = 0;

private:
    NoiseState state;

    double scaling;
    double height;
    double step_scaling;
    double step_height;
    double slope;
    double ridge;
};

}
}

#endif // FRACTALNOISE_H
