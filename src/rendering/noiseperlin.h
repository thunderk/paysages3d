#ifndef _PAYSAGES_NOISEPERLIN_H_
#define _PAYSAGES_NOISEPERLIN_H_

#ifdef __cplusplus
extern "C" {
#endif

void noisePerlinInit();
double noisePerlinGet1DValue(double x);
double noisePerlinGet2DValue(double x, double y);
double noisePerlinGet3DValue(double x, double y, double z);
/*double noiseSimplexGet4DValue(double x, double y, double z, double w);*/
    
#ifdef __cplusplus
}
#endif

#endif
