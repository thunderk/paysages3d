#ifndef _PAYSAGES_AUTO_H_
#define _PAYSAGES_AUTO_H_

#include "renderer.h"

#ifdef __cplusplus
extern "C" {
#endif

void autoSetDaytime(int hour, int minute);
void autoSetDaytimeFraction(float daytime);
void autoGenRealisticLandscape(int seed);

#ifdef __cplusplus
}
#endif

#endif
