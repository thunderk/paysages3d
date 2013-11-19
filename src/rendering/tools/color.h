#ifndef _PAYSAGES_TOOLS_COLOR_H_
#define _PAYSAGES_TOOLS_COLOR_H_

#include "../rendering_global.h"

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
