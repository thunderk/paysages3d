#ifndef _PAYSAGES_TOOLS_COLOR_H_
#define _PAYSAGES_TOOLS_COLOR_H_

#include "../rendering_global.h"

/* HDR profile for tone-mapping */
class ColorProfile;
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


// TEMP
#include "Color.h"

#endif
