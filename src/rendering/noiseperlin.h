#ifndef _PAYSAGES_NOISEPERLIN_H_
#define _PAYSAGES_NOISEPERLIN_H_

#include "rendering_global.h"

#ifdef __cplusplus
extern "C" {
#endif

RENDERINGSHARED_EXPORT void noisePerlinInit();
RENDERINGSHARED_EXPORT double noisePerlinGet1DValue(double x);
RENDERINGSHARED_EXPORT double noisePerlinGet2DValue(double x, double y);
RENDERINGSHARED_EXPORT double noisePerlinGet3DValue(double x, double y, double z);
/*double noiseSimplexGet4DValue(double x, double y, double z, double w);*/
    
#ifdef __cplusplus
}
#endif

#endif
