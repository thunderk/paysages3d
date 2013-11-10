#ifndef NOISEFUNCTIONPERLIN_H
#define NOISEFUNCTIONPERLIN_H

#include "basics_global.h"

namespace paysages {
namespace system {
    class PackStream;
}
namespace basics {

class NoiseFunctionPerlin
{
public:
    NoiseFunctionPerlin();
};

}
}

void noisePerlinInit();
double noisePerlinGet1DValue(double x);
double noisePerlinGet2DValue(double x, double y);
double noisePerlinGet3DValue(double x, double y, double z);
/*double noiseSimplexGet4DValue(double x, double y, double z, double w);*/

#endif // NOISEFUNCTIONPERLIN_H