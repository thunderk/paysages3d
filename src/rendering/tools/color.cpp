#include "color.h"

#include <cstdlib>
#include <cassert>
#include <cstring>
#include <cmath>
#include "PackStream.h"
#include "Curve.h"

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

static Color _toneMappingClamp(Color pixel, double)
{
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
    result->red = new Curve;
    result->green = new Curve;
    result->blue = new Curve;

    return result;
}

void colorGradationDelete(ColorGradation* gradation)
{
    delete gradation->red;
    delete gradation->green;
    delete gradation->blue;
    delete gradation;
}

void colorGradationCopy(ColorGradation* source, ColorGradation* destination)
{
    source->red->copy(destination->red);
    source->green->copy(destination->green);
    source->blue->copy(destination->blue);
}

void colorGradationClear(ColorGradation* gradation)
{
    gradation->red->clear();
    gradation->green->clear();
    gradation->blue->clear();
}

void colorGradationSave(PackStream* stream, ColorGradation* gradation)
{
    gradation->red->save(stream);
    gradation->green->save(stream);
    gradation->blue->save(stream);
}

void colorGradationLoad(PackStream* stream, ColorGradation* gradation)
{
    gradation->red->load(stream);
    gradation->green->load(stream);
    gradation->blue->load(stream);
}

void colorGradationGetRedCurve(ColorGradation* gradation, Curve* curve)
{
    gradation->red->copy(curve);
}

void colorGradationGetGreenCurve(ColorGradation* gradation, Curve* curve)
{
    gradation->green->copy(curve);
}

void colorGradationGetBlueCurve(ColorGradation* gradation, Curve* curve)
{
    gradation->blue->copy(curve);
}

void colorGradationSetRedCurve(ColorGradation* gradation, Curve* curve)
{
    curve->copy(gradation->red);
    gradation->red->validate();
}

void colorGradationSetGreenCurve(ColorGradation* gradation, Curve* curve)
{
    curve->copy(gradation->green);
    gradation->green->validate();
}

void colorGradationSetBlueCurve(ColorGradation* gradation, Curve* curve)
{
    curve->copy(gradation->blue);
    gradation->blue->validate();
}

void colorGradationQuickAdd(ColorGradation* gradation, double value, Color* col)
{
    colorGradationQuickAddRgb(gradation, value, col->r, col->g, col->b);
}

void colorGradationQuickAddRgb(ColorGradation* gradation, double value, double r, double g, double b)
{
    gradation->red->addPoint(value, r);
    gradation->red->validate();

    gradation->green->addPoint(value, g);
    gradation->green->validate();

    gradation->blue->addPoint(value, b);
    gradation->blue->validate();
}

Color colorGradationGet(ColorGradation* gradation, double value)
{
    Color result;

    result.r = gradation->red->getValue(value);
    result.g = gradation->green->getValue(value);
    result.b = gradation->blue->getValue(value);
    result.a = 1.0;

    return result;
}
