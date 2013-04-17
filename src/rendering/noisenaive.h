#ifndef _PAYSAGES_NOISENAIVE_H_
#define _PAYSAGES_NOISENAIVE_H_

#include "tools/pack.h"

#ifdef __cplusplus
extern "C" {
#endif

void noiseNaiveInit();
void noiseNaiveQuit();
void noiseNaiveSave(PackStream* stream);
void noiseNaiveLoad(PackStream* stream);
double noiseNaiveGet1DValue(double x);
double noiseNaiveGet2DValue(double x, double y);
double noiseNaiveGet3DValue(double x, double y, double z);
/*double noiseNaiveGet4DValue(double x, double y, double z, double w);*/

#ifdef __cplusplus
}
#endif

#endif
