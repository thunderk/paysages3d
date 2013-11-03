#ifndef NOISEFUNCTIONNAIVE_H
#define NOISEFUNCTIONNAIVE_H

#include "basics_global.h"

namespace paysages {
namespace system {
    class PackStream;
}
namespace basics {

class NoiseFunctionNaive
{
public:
    NoiseFunctionNaive();
};

}
}

void noiseNaiveInit();
void noiseNaiveQuit();
void noiseNaiveSave(PackStream* stream);
void noiseNaiveLoad(PackStream* stream);
double noiseNaiveGet1DValue(double x);
double noiseNaiveGet2DValue(double x, double y);
double noiseNaiveGet3DValue(double x, double y, double z);
/*double noiseNaiveGet4DValue(double x, double y, double z, double w);*/

#endif // NOISEFUNCTIONNAIVE_H
