#ifndef _PAYSAGES_SIMPLEXNOISE_H_
#define _PAYSAGES_SIMPLEXNOISE_H_

#ifdef __cplusplus
extern "C" {
#endif

void simplexNoiseInit();
double simplexNoiseGet2DValue(double xin, double yin);
double simplexNoiseGet3DValue(double xin, double yin, double zin);
double simplexNoiseGet4DValue(double x, double y, double z, double w);
    
#ifdef __cplusplus
}
#endif

#endif
