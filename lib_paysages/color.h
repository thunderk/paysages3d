#ifndef _PAYSAGES_COLOR_H_
#define _PAYSAGES_COLOR_H_

#include "curve.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    float r;
    float g;
    float b;
    float a;
} Color;

extern Color COLOR_TRANSPARENT;
extern Color COLOR_BLACK;
extern Color COLOR_RED;
extern Color COLOR_GREEN;
extern Color COLOR_BLUE;
extern Color COLOR_WHITE;
extern Color COLOR_GREY;

/* Color */
void colorSave(PackStream* stream, Color* col);
void colorLoad(PackStream* stream, Color* col);

unsigned int colorTo32BitRGBA(Color* col);
unsigned int colorTo32BitBGRA(Color* col);
unsigned int colorTo32BitARGB(Color* col);
unsigned int colorTo32BitABGR(Color* col);

void colorMask(Color* base, Color* mask);
float colorNormalize(Color* col);
float colorGetValue(Color* col);

/* ColorGradation */
typedef struct ColorGradation ColorGradation;

ColorGradation* colorGradationCreate();
void colorGradationDelete(ColorGradation* gradation);
void colorGradationCopy(ColorGradation* source, ColorGradation* destination);

void colorGradationSave(PackStream* stream, ColorGradation* gradation);
void colorGradationLoad(PackStream* stream, ColorGradation* gradation);

void colorGradationGetRedCurve(ColorGradation* gradation, Curve* curve);
void colorGradationGetGreenCurve(ColorGradation* gradation, Curve* curve);
void colorGradationGetBlueCurve(ColorGradation* gradation, Curve* curve);

void colorGradationSetRedCurve(ColorGradation* gradation, Curve* curve);
void colorGradationSetGreenCurve(ColorGradation* gradation, Curve* curve);
void colorGradationSetBlueCurve(ColorGradation* gradation, Curve* curve);

void colorGradationQuickAdd(ColorGradation* gradation, float value, Color* col);
void colorGradationQuickAddRgb(ColorGradation* gradation, float value, float r, float g, float b);

Color colorGradationGet(ColorGradation* gradation, float value);

#ifdef __cplusplus
}
#endif

#endif
