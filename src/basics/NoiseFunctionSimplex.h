#ifndef NOISEFUNCTIONSIMPLEX_H
#define NOISEFUNCTIONSIMPLEX_H

#include "basics_global.h"

namespace paysages {
namespace system {
    class PackStream;
}
namespace basics {

class NoiseFunctionSimplex
{
public:
    NoiseFunctionSimplex();
};

}
}

void noiseSimplexInit();
double noiseSimplexGet1DValue(double x);
double noiseSimplexGet2DValue(double x, double y);
double noiseSimplexGet3DValue(double x, double y, double z);
double noiseSimplexGet4DValue(double x, double y, double z, double w);

#endif // NOISEFUNCTIONSIMPLEX_H
