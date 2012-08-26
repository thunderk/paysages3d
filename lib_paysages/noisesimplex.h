#ifndef _PAYSAGES_SIMPLEXNOISE_H_
#define _PAYSAGES_SIMPLEXNOISE_H_

#ifdef __cplusplus
extern "C" {
#endif

void noiseSimplexInit();
double noiseSimplexGet2DValue(double xin, double yin);
double noiseSimplexGet3DValue(double xin, double yin, double zin);
double noiseSimplexGet4DValue(double x, double y, double z, double w);
    
#ifdef __cplusplus
}
#endif

#endif
