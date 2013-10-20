#ifndef _PAYSAGES_NOISENAIVE_H_
#define _PAYSAGES_NOISENAIVE_H_

#include "rendering_global.h"
#include "tools/pack.h"

#ifdef __cplusplus
extern "C" {
#endif

RENDERINGSHARED_EXPORT void noiseNaiveInit();
RENDERINGSHARED_EXPORT void noiseNaiveQuit();
RENDERINGSHARED_EXPORT void noiseNaiveSave(PackStream* stream);
RENDERINGSHARED_EXPORT void noiseNaiveLoad(PackStream* stream);
RENDERINGSHARED_EXPORT double noiseNaiveGet1DValue(double x);
RENDERINGSHARED_EXPORT double noiseNaiveGet2DValue(double x, double y);
RENDERINGSHARED_EXPORT double noiseNaiveGet3DValue(double x, double y, double z);
/*double noiseNaiveGet4DValue(double x, double y, double z, double w);*/

#ifdef __cplusplus
}
#endif

#endif
