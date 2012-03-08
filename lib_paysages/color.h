#ifndef _PAYSAGES_COLOR_H_
#define _PAYSAGES_COLOR_H_

#include "shared/types.h"
#include "shared/constants.h"
#include "curve.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Color */
void colorSave(FILE* f, Color* col);
void colorLoad(FILE* f, Color* col);

unsigned int colorTo32BitRGBA(Color* col);
unsigned int colorTo32BitBGRA(Color* col);
unsigned int colorTo32BitARGB(Color* col);
unsigned int colorTo32BitABGR(Color* col);

void colorMask(Color* base, Color* mask);
double colorNormalize(Color* col);
double colorGetValue(Color* col);

/* ColorGradation */
typedef struct ColorGradation ColorGradation;

ColorGradation* colorGradationCreate();
void colorGradationDelete(ColorGradation* gradation);
void colorGradationCopy(ColorGradation* source, ColorGradation* destination);

void colorGradationSave(FILE* f, ColorGradation* gradation);
void colorGradationLoad(FILE* f, ColorGradation* gradation);

void colorGradationGetRedCurve(ColorGradation* gradation, Curve* curve);
void colorGradationGetGreenCurve(ColorGradation* gradation, Curve* curve);
void colorGradationGetBlueCurve(ColorGradation* gradation, Curve* curve);

void colorGradationSetRedCurve(ColorGradation* gradation, Curve* curve);
void colorGradationSetGreenCurve(ColorGradation* gradation, Curve* curve);
void colorGradationSetBlueCurve(ColorGradation* gradation, Curve* curve);

void colorGradationQuickAdd(ColorGradation* gradation, double value, Color* col);
void colorGradationQuickAddRgb(ColorGradation* gradation, double value, double r, double g, double b);

Color colorGradationGet(ColorGradation* gradation, double value);

#ifdef __cplusplus
}
#endif

#endif
