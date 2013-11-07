#ifndef _PAYSAGES_TOOLS_COLOR_H_
#define _PAYSAGES_TOOLS_COLOR_H_

#include "../rendering_global.h"
#include "curve.h"

namespace paysages {
namespace system {class PackStream;}
}

#define TYPEDEF_COLOR
typedef struct
{
    double r;
    double g;
    double b;
    double a;
} Color;

RENDERINGSHARED_EXPORT extern Color COLOR_TRANSPARENT;
RENDERINGSHARED_EXPORT extern Color COLOR_BLACK;
RENDERINGSHARED_EXPORT extern Color COLOR_RED;
RENDERINGSHARED_EXPORT extern Color COLOR_GREEN;
RENDERINGSHARED_EXPORT extern Color COLOR_BLUE;
RENDERINGSHARED_EXPORT extern Color COLOR_WHITE;
RENDERINGSHARED_EXPORT extern Color COLOR_GREY;

/* Color */
RENDERINGSHARED_EXPORT void colorSave(PackStream* stream, Color* col);
RENDERINGSHARED_EXPORT void colorLoad(PackStream* stream, Color* col);
RENDERINGSHARED_EXPORT Color colorFromValues(double r, double g, double b, double a);

RENDERINGSHARED_EXPORT unsigned int colorTo32BitRGBA(Color* col);
RENDERINGSHARED_EXPORT unsigned int colorTo32BitBGRA(Color* col);
RENDERINGSHARED_EXPORT unsigned int colorTo32BitARGB(Color* col);
RENDERINGSHARED_EXPORT unsigned int colorTo32BitABGR(Color* col);

RENDERINGSHARED_EXPORT Color colorFrom32BitRGBA(unsigned int col);
RENDERINGSHARED_EXPORT Color colorFrom32BitBGRA(unsigned int col);
RENDERINGSHARED_EXPORT Color colorFrom32BitARGB(unsigned int col);
RENDERINGSHARED_EXPORT Color colorFrom32BitABGR(unsigned int col);

RENDERINGSHARED_EXPORT void colorMask(Color* base, Color* mask);
RENDERINGSHARED_EXPORT double colorNormalize(Color* col);
RENDERINGSHARED_EXPORT double colorGetValue(Color* col);

RENDERINGSHARED_EXPORT double colorGetPower(Color* col);
RENDERINGSHARED_EXPORT void colorLimitPower(Color* col, double max_power);

/* HDR profile for tone-mapping */
typedef struct ColorProfile ColorProfile;
typedef enum
{
    TONE_MAPPING_UNCHARTED,
    TONE_MAPPING_REIHNARD,
    TONE_MAPPING_CLAMP
} ToneMappingOperator;

RENDERINGSHARED_EXPORT ColorProfile* colorProfileCreate();
RENDERINGSHARED_EXPORT void colorProfileDelete(ColorProfile* profile);

RENDERINGSHARED_EXPORT void colorProfileSetToneMapping(ColorProfile* profile, ToneMappingOperator tonemapper, double exposure);

RENDERINGSHARED_EXPORT void colorProfileSave(PackStream* stream, ColorProfile* profile);
RENDERINGSHARED_EXPORT void colorProfileLoad(PackStream* stream, ColorProfile* profile);

RENDERINGSHARED_EXPORT void colorProfileClear(ColorProfile* profile);
RENDERINGSHARED_EXPORT int colorProfileCollect(ColorProfile* profile, Color pixel);
RENDERINGSHARED_EXPORT Color colorProfileApply(ColorProfile* profile, Color pixel);

/* ColorGradation */
typedef struct ColorGradation ColorGradation;

RENDERINGSHARED_EXPORT ColorGradation* colorGradationCreate();
RENDERINGSHARED_EXPORT void colorGradationDelete(ColorGradation* gradation);
RENDERINGSHARED_EXPORT void colorGradationCopy(ColorGradation* source, ColorGradation* destination);
RENDERINGSHARED_EXPORT void colorGradationClear(ColorGradation* gradation);

RENDERINGSHARED_EXPORT void colorGradationSave(PackStream* stream, ColorGradation* gradation);
RENDERINGSHARED_EXPORT void colorGradationLoad(PackStream* stream, ColorGradation* gradation);

RENDERINGSHARED_EXPORT void colorGradationGetRedCurve(ColorGradation* gradation, Curve* curve);
RENDERINGSHARED_EXPORT void colorGradationGetGreenCurve(ColorGradation* gradation, Curve* curve);
RENDERINGSHARED_EXPORT void colorGradationGetBlueCurve(ColorGradation* gradation, Curve* curve);

RENDERINGSHARED_EXPORT void colorGradationSetRedCurve(ColorGradation* gradation, Curve* curve);
RENDERINGSHARED_EXPORT void colorGradationSetGreenCurve(ColorGradation* gradation, Curve* curve);
RENDERINGSHARED_EXPORT void colorGradationSetBlueCurve(ColorGradation* gradation, Curve* curve);

RENDERINGSHARED_EXPORT void colorGradationQuickAdd(ColorGradation* gradation, double value, Color* col);
RENDERINGSHARED_EXPORT void colorGradationQuickAddRgb(ColorGradation* gradation, double value, double r, double g, double b);

RENDERINGSHARED_EXPORT Color colorGradationGet(ColorGradation* gradation, double value);

/* HSL color space */
typedef struct
{
    double h;
    double s;
    double l;
    double a;
} ColorHSL;

RENDERINGSHARED_EXPORT Color colorFromHSL(ColorHSL col);
RENDERINGSHARED_EXPORT ColorHSL colorToHSL(Color col);

RENDERINGSHARED_EXPORT ColorHSL colorHSLFromValues(double h, double s, double l, double a);

#endif
