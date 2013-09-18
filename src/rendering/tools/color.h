#ifndef _PAYSAGES_TOOLS_COLOR_H_
#define _PAYSAGES_TOOLS_COLOR_H_

#include "curve.h"
#include "pack.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    double r;
    double g;
    double b;
    double a;
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
Color colorFromValues(double r, double g, double b, double a);

unsigned int colorTo32BitRGBA(Color* col);
unsigned int colorTo32BitBGRA(Color* col);
unsigned int colorTo32BitARGB(Color* col);
unsigned int colorTo32BitABGR(Color* col);

Color colorFrom32BitRGBA(unsigned int col);
Color colorFrom32BitBGRA(unsigned int col);
Color colorFrom32BitARGB(unsigned int col);
Color colorFrom32BitABGR(unsigned int col);

void colorMask(Color* base, Color* mask);
double colorNormalize(Color* col);
double colorGetValue(Color* col);

double colorGetPower(Color* col);
void colorLimitPower(Color* col, double max_power);

/* HDR profile for tone-mapping */
typedef struct ColorProfile ColorProfile;
typedef enum
{
    TONE_MAPPING_UNCHARTED,
    TONE_MAPPING_REIHNARD,
    TONE_MAPPING_CLAMP
} ToneMappingOperator;

ColorProfile* colorProfileCreate();
void colorProfileDelete(ColorProfile* profile);

void colorProfileSetToneMapping(ColorProfile* profile, ToneMappingOperator tonemapper, double exposure);

void colorProfileSave(PackStream* stream, ColorProfile* profile);
void colorProfileLoad(PackStream* stream, ColorProfile* profile);

void colorProfileClear(ColorProfile* profile);
int colorProfileCollect(ColorProfile* profile, Color pixel);
Color colorProfileApply(ColorProfile* profile, Color pixel);

/* ColorGradation */
typedef struct ColorGradation ColorGradation;

ColorGradation* colorGradationCreate();
void colorGradationDelete(ColorGradation* gradation);
void colorGradationCopy(ColorGradation* source, ColorGradation* destination);
void colorGradationClear(ColorGradation* gradation);

void colorGradationSave(PackStream* stream, ColorGradation* gradation);
void colorGradationLoad(PackStream* stream, ColorGradation* gradation);

void colorGradationGetRedCurve(ColorGradation* gradation, Curve* curve);
void colorGradationGetGreenCurve(ColorGradation* gradation, Curve* curve);
void colorGradationGetBlueCurve(ColorGradation* gradation, Curve* curve);

void colorGradationSetRedCurve(ColorGradation* gradation, Curve* curve);
void colorGradationSetGreenCurve(ColorGradation* gradation, Curve* curve);
void colorGradationSetBlueCurve(ColorGradation* gradation, Curve* curve);

void colorGradationQuickAdd(ColorGradation* gradation, double value, Color* col);
void colorGradationQuickAddRgb(ColorGradation* gradation, double value, double r, double g, double b);

Color colorGradationGet(ColorGradation* gradation, double value);

/* HSL color space */
typedef struct
{
    double h;
    double s;
    double l;
    double a;
} ColorHSL;

Color colorFromHSL(ColorHSL col);
ColorHSL colorToHSL(Color col);

ColorHSL colorHSLFromValues(double h, double s, double l, double a);

#ifdef __cplusplus
}
#endif

#endif
