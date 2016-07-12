#pragma once

#include "basics_global.h"

namespace paysages {
namespace basics {

class BASICSSHARED_EXPORT NoiseFunctionPerlin {
  public:
    NoiseFunctionPerlin();
};
}
}

double noisePerlinGet1DValue(double x);
double noisePerlinGet2DValue(double x, double y);
double noisePerlinGet3DValue(double x, double y, double z);
/*double noiseSimplexGet4DValue(double x, double y, double z, double w);*/
