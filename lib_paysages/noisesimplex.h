#ifndef _PAYSAGES_NOISESIMPLEX_H_
#define _PAYSAGES_NOISESIMPLEX_H_

#ifdef __cplusplus
extern "C" {
#endif

void noiseSimplexInit();
double noiseSimplexGet1DValue(double x);
double noiseSimplexGet2DValue(double x, double y);
double noiseSimplexGet3DValue(double x, double y, double z);
double noiseSimplexGet4DValue(double x, double y, double z, double w);
    
#ifdef __cplusplus
}
#endif

#endif
