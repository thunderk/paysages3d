#ifndef NOISEFUNCTIONSIMPLEX_H
#define NOISEFUNCTIONSIMPLEX_H

#include "basics_global.h"

#include "FractalNoise.h"

namespace paysages {
namespace basics {

class NoiseFunctionSimplex : public FractalNoise {
    virtual double getBase2d(double x, double y) const override;
    virtual double getBase3d(double x, double y, double z) const override;

  public:
    static const Texture2D *getValueTexture();
    static const Texture2D *getNormalTexture();
};
}
}

double noiseSimplexGet1DValue(double x);
double noiseSimplexGet2DValue(double x, double y);
double noiseSimplexGet3DValue(double x, double y, double z);
double noiseSimplexGet4DValue(double x, double y, double z, double w);

#endif // NOISEFUNCTIONSIMPLEX_H
