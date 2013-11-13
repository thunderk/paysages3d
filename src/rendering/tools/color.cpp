#include "color.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "../tools.h"
#include "PackStream.h"

/******************************** ColorProfile ********************************/
class ColorProfile
{
public:
    double minvalue;
    double maxvalue;
    Color(*mapper)(Color col, double exposure);
    double exposure;
};

ColorProfile* colorProfileCreate()
{
    ColorProfile* profile;

    profile = new ColorProfile;

    colorProfileSetToneMapping(profile, TONE_MAPPING_UNCHARTED, 2.0);
    colorProfileClear(profile);

    return profile;
}

void colorProfileDelete(ColorProfile* profile)
{
    delete profile;
}

static inline double _uncharted2Tonemap(double x)
{
    double A = 0.15;
    double B = 0.50;
    double C = 0.10;
    double D = 0.20;
    double E = 0.02;
    double F = 0.30;

    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

static Color _toneMappingUncharted(Color pixel, double exposure)
{
    double W = 11.2;
    double white_scale = 1.0 / _uncharted2Tonemap(W);

    pixel.r = pow(_uncharted2Tonemap(pixel.r * exposure) * white_scale, 1.0 / 2.2);
    pixel.g = pow(_uncharted2Tonemap(pixel.g * exposure) * white_scale, 1.0 / 2.2);
    pixel.b = pow(_uncharted2Tonemap(pixel.b * exposure) * white_scale, 1.0 / 2.2);

    return pixel;
}

static Color _toneMappingReinhard(Color pixel, double exposure)
{
    pixel.r = (pixel.r * exposure) / (1.0 + pixel.r * exposure);
    pixel.g = (pixel.g * exposure) / (1.0 + pixel.g * exposure);
    pixel.b = (pixel.b * exposure) / (1.0 + pixel.b * exposure);

    return pixel;
}

static Color _toneMappingClamp(Color pixel, double exposure)
{
    UNUSED(exposure);

    pixel.r = pixel.r > 1.0 ? 1.0 : pixel.r;
    pixel.g = pixel.g > 1.0 ? 1.0 : pixel.g;
    pixel.b = pixel.b > 1.0 ? 1.0 : pixel.b;

    return pixel;
}

void colorProfileSetToneMapping(ColorProfile* profile, ToneMappingOperator tonemapper, double exposure)
{
    switch (tonemapper)
    {
    case TONE_MAPPING_REIHNARD:
        profile->mapper = _toneMappingReinhard;
        break;
    case TONE_MAPPING_UNCHARTED:
        profile->mapper = _toneMappingUncharted;
        break;
    default:
        profile->mapper = _toneMappingClamp;
    }
    profile->exposure = exposure;
}

void colorProfileSave(PackStream*, ColorProfile*)
{
    /* TODO */
}

void colorProfileLoad(PackStream*, ColorProfile*)
{
    /* TODO */
}

void colorProfileClear(ColorProfile* profile)
{
    profile->minvalue = 0.0;
    profile->maxvalue = 3.0;
}

int colorProfileCollect(ColorProfile* profile, Color pixel)
{
    int changed = 0;
    double value = pixel.r + pixel.g + pixel.b;

    if (value < profile->minvalue)
    {
        profile->minvalue = value;
        changed = 1;
    }
    if (value > profile->maxvalue)
    {
        profile->maxvalue = value;
        changed = 1;
    }
    return changed;
}

Color colorProfileApply(ColorProfile* profile, Color pixel)
{
    return profile->mapper(pixel, profile->exposure);
}

/******************************** ColorGradation ********************************/
struct ColorGradation
{
    Curve* red;
    Curve* green;
    Curve* blue;
};

ColorGradation* colorGradationCreate()
{
    ColorGradation* result;

    result = new ColorGradation;
    result->red = curveCreate();
    result->green = curveCreate();
    result->blue = curveCreate();

    return result;
}

void colorGradationDelete(ColorGradation* gradation)
{
    curveDelete(gradation->red);
    curveDelete(gradation->green);
    curveDelete(gradation->blue);
    delete gradation;
}

void colorGradationCopy(ColorGradation* source, ColorGradation* destination)
{
    curveCopy(source->red, destination->red);
    curveCopy(source->green, destination->green);
    curveCopy(source->blue, destination->blue);
}

void colorGradationClear(ColorGradation* gradation)
{
    curveClear(gradation->red);
    curveClear(gradation->green);
    curveClear(gradation->blue);
}

void colorGradationSave(PackStream* stream, ColorGradation* gradation)
{
    curveSave(stream, gradation->red);
    curveSave(stream, gradation->green);
    curveSave(stream, gradation->blue);
}

void colorGradationLoad(PackStream* stream, ColorGradation* gradation)
{
    curveLoad(stream, gradation->red);
    curveLoad(stream, gradation->green);
    curveLoad(stream, gradation->blue);
}

void colorGradationGetRedCurve(ColorGradation* gradation, Curve* curve)
{
    curveCopy(gradation->red, curve);
}

void colorGradationGetGreenCurve(ColorGradation* gradation, Curve* curve)
{
    curveCopy(gradation->green, curve);
}

void colorGradationGetBlueCurve(ColorGradation* gradation, Curve* curve)
{
    curveCopy(gradation->blue, curve);
}

void colorGradationSetRedCurve(ColorGradation* gradation, Curve* curve)
{
    curveCopy(curve, gradation->red);
    curveValidate(gradation->red);
}

void colorGradationSetGreenCurve(ColorGradation* gradation, Curve* curve)
{
    curveCopy(curve, gradation->green);
    curveValidate(gradation->green);
}

void colorGradationSetBlueCurve(ColorGradation* gradation, Curve* curve)
{
    curveCopy(curve, gradation->blue);
    curveValidate(gradation->blue);
}

void colorGradationQuickAdd(ColorGradation* gradation, double value, Color* col)
{
    colorGradationQuickAddRgb(gradation, value, col->r, col->g, col->b);
}

void colorGradationQuickAddRgb(ColorGradation* gradation, double value, double r, double g, double b)
{
    curveQuickAddPoint(gradation->red, value, r);
    curveValidate(gradation->red);

    curveQuickAddPoint(gradation->green, value, g);
    curveValidate(gradation->green);

    curveQuickAddPoint(gradation->blue, value, b);
    curveValidate(gradation->blue);
}

Color colorGradationGet(ColorGradation* gradation, double value)
{
    Color result;

    result.r = curveGetValue(gradation->red, value);
    result.g = curveGetValue(gradation->green, value);
    result.b = curveGetValue(gradation->blue, value);
    result.a = 1.0;

    return result;
}
