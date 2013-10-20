#ifndef _PAYSAGES_NOISESIMPLEX_H_
#define _PAYSAGES_NOISESIMPLEX_H_

#include "rendering_global.h"

#ifdef __cplusplus
extern "C" {
#endif

RENDERINGSHARED_EXPORT void noiseSimplexInit();
RENDERINGSHARED_EXPORT double noiseSimplexGet1DValue(double x);
RENDERINGSHARED_EXPORT double noiseSimplexGet2DValue(double x, double y);
RENDERINGSHARED_EXPORT double noiseSimplexGet3DValue(double x, double y, double z);
RENDERINGSHARED_EXPORT double noiseSimplexGet4DValue(double x, double y, double z, double w);
    
#ifdef __cplusplus
}
#endif

#endif
