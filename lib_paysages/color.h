#ifndef _PAYSAGES_COLOR_H_
#define _PAYSAGES_COLOR_H_

#include "shared/types.h"

#ifdef __cplusplus
extern "C" {
#endif

void colorSave(FILE* f, Color* col);
void colorLoad(FILE* f, Color* col);
unsigned int colorTo32BitRGBA(Color* col);
unsigned int colorTo32BitBGRA(Color* col);
unsigned int colorTo32BitARGB(Color* col);
unsigned int colorTo32BitABGR(Color* col);
void colorMask(Color* base, Color* mask);
double colorNormalize(Color* col);
double colorGetValue(Color* col);

ColorGradation colorGradationCreate();
void colorGradationSave(FILE* f, ColorGradation* gradation);
void colorGradationLoad(FILE* f, ColorGradation* gradation);
void colorGradationAdd(ColorGradation* gradation, double value, Color* col);
void colorGradationAddRgba(ColorGradation* gradation, double value, double r, double g, double b, double a);
Color colorGradationGet(ColorGradation* gradation, double value);

#ifdef __cplusplus
}
#endif

#endif
