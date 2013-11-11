#include "color.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "../tools.h"
#include "PackStream.h"

/******************************** Color ********************************/

Color COLOR_TRANSPARENT = {0.0, 0.0, 0.0, 0.0};
Color COLOR_BLACK = {0.0, 0.0, 0.0, 1.0};
Color COLOR_RED = {1.0, 0.0, 0.0, 1.0};
Color COLOR_GREEN = {0.0, 1.0, 0.0, 1.0};
Color COLOR_BLUE = {0.0, 0.0, 1.0, 1.0};
Color COLOR_WHITE = {1.0, 1.0, 1.0, 1.0};
Color COLOR_GREY = {0.5, 0.5, 0.5, 1.0};

void colorSave(PackStream* stream, Color* col)
{
    stream->write(&col->r);
    stream->write(&col->g);
    stream->write(&col->b);
    stream->write(&col->a);
}

void colorLoad(PackStream* stream, Color* col)
{
    stream->read(&col->r);
    stream->read(&col->g);
    stream->read(&col->b);
    stream->read(&col->a);
}

Color colorFromValues(double r, double g, double b, double a)
{
    Color result;

    result.r = r;
    result.g = g;
    result.b = b;
    result.a = a;

    return result;
}

unsigned int colorTo32BitRGBA(Color* col)
{
    return (((unsigned int) (col->a * 255.0)) << 24) | (((unsigned int) (col->b * 255.0)) << 16) | (((unsigned int) (col->g * 255.0)) << 8) | ((unsigned int) (col->r * 255.0));
}

unsigned int colorTo32BitBGRA(Color* col)
{
    return (((unsigned int) (col->a * 255.0)) << 24) | (((unsigned int) (col->r * 255.0)) << 16) | (((unsigned int) (col->g * 255.0)) << 8) | ((unsigned int) (col->b * 255.0));
}

unsigned int colorTo32BitARGB(Color* col)
{
    return (((unsigned int) (col->b * 255.0)) << 24) | (((unsigned int) (col->g * 255.0)) << 16) | (((unsigned int) (col->r * 255.0)) << 8) | ((unsigned int) (col->a * 255.0));
}

unsigned int colorTo32BitABGR(Color* col)
{
    return (((unsigned int) (col->r * 255.0)) << 24) | (((unsigned int) (col->g * 255.0)) << 16) | (((unsigned int) (col->b * 255.0)) << 8) | ((unsigned int) (col->a * 255.0));
}

Color colorFrom32BitRGBA(unsigned int col)
{
    Color result;

    result.r = ((double) (col & 0x000000FF)) / 255.0;
    result.g = ((double) ((col & 0x0000FF00) >> 8)) / 255.0;
    result.b = ((double) ((col & 0x00FF0000) >> 16)) / 255.0;
    result.a = ((double) ((col & 0xFF000000) >> 24)) / 255.0;

    return result;
}

Color colorFrom32BitBGRA(unsigned int col)
{
    Color result;

    result.b = ((double) (col & 0x000000FF)) / 255.0;
    result.g = ((double) ((col & 0x0000FF00) >> 8)) / 255.0;
    result.r = ((double) ((col & 0x00FF0000) >> 16)) / 255.0;
    result.a = ((double) ((col & 0xFF000000) >> 24)) / 255.0;

    return result;
}

Color colorFrom32BitARGB(unsigned int col)
{
    Color result;

    result.a = ((double) (col & 0x000000FF)) / 255.0;
    result.r = ((double) ((col & 0x0000FF00) >> 8)) / 255.0;
    result.g = ((double) ((col & 0x00FF0000) >> 16)) / 255.0;
    result.b = ((double) ((col & 0xFF000000) >> 24)) / 255.0;

    return result;
}

Color colorFrom32BitABGR(unsigned int col)
{
    Color result;

    result.a = ((double) (col & 0x000000FF)) / 255.0;
    result.b = ((double) ((col & 0x0000FF00) >> 8)) / 255.0;
    result.g = ((double) ((col & 0x00FF0000) >> 16)) / 255.0;
    result.r = ((double) ((col & 0xFF000000) >> 24)) / 255.0;

    return result;
}

void colorMask(Color* base, Color* mask)
{
    double new_a;
    new_a = base->a + mask->a - (base->a * mask->a);
    base->r = (mask->r * mask->a + base->r * base->a - base->r * base->a * mask->a) / new_a;
    base->g = (mask->g * mask->a + base->g * base->a - base->g * base->a * mask->a) / new_a;
    base->b = (mask->b * mask->a + base->b * base->a - base->b * base->a * mask->a) / new_a;
    base->a = new_a;

    /*double mask_weight = mask->a;
    double base_weight = 1.0 - mask_weight;

    base->r = mask->r * mask_weight + base->r * base_weight;
    base->g = mask->g * mask_weight + base->g * base_weight;
    base->b = mask->b * mask_weight + base->b * base_weight;
    base->a = base->a + mask_weight * (1.0 - base->a);*/
}

double colorNormalize(Color* col)
{
    assert(col->r >= 0.0);
    assert(col->g >= 0.0);
    assert(col->b >= 0.0);
    assert(col->a >= 0.0);
#ifdef isnan
    assert(!isnan(col->r));
    assert(!isnan(col->g));
    assert(!isnan(col->b));
    assert(!isnan(col->a));
#endif
#ifdef isfinite
    assert(isfinite(col->r));
    assert(isfinite(col->g));
    assert(isfinite(col->b));
    assert(isfinite(col->a));
#endif

    if (col->r > 1.0)
    {
        col->r = 1.0;
    }
    if (col->g > 1.0)
    {
        col->g = 1.0;
    }
    if (col->b > 1.0)
    {
        col->b = 1.0;
    }
    return 1.0;
    /*double max = colorGetValue(col);

    assert(max >= 0.0);

    if (max > 1.0)
    {
        col->r /= max;
        col->g /= max;
        col->b /= max;
    }
    return max;*/
}

double colorGetValue(Color* col)
{
    double max;

    max = col->r;
    if (col->g > max)
    {
        max = col->g;
    }
    if (col->b > max)
    {
        max = col->b;
    }
    return max;
}

double colorGetPower(Color* col)
{
    return col->r + col->g + col->b;
}

void colorLimitPower(Color* col, double max_power)
{
    double power = colorGetPower(col);

    if (power > max_power)
    {
        double factor = max_power / power;

        col->r *= factor;
        col->g *= factor;
        col->b *= factor;
    }
}

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

/******************************** HSL Color Space ********************************/
static double _hue2rgb(double p, double q, double t)
{
    if (t < 0.0) t += 1;
    if (t > 1.0) t -= 1;
    if (t < 1.0 / 6.0) return p + (q - p) * 6.0 * t;
    if (t < 1.0 / 2.0) return q;
    if (t < 2.0 / 3.0) return p + (q - p) * (2.0 / 3.0 - t) * 6.0;
    return p;
}

Color colorFromHSL(ColorHSL col)
{
    Color result;

    if (col.s == 0)
    {
        result.r = result.g = result.b = col.l;
    }
    else
    {
        double q = col.l < 0.5 ? col.l * (1.0 + col.s) : col.l + col.s - col.l * col.s;
        double p = 2 * col.l - q;
        result.r = _hue2rgb(p, q, col.h + 1.0 / 3.0);
        result.g = _hue2rgb(p, q, col.h);
        result.b = _hue2rgb(p, q, col.h - 1.0 / 3.0);
    }

    result.a = col.a;

    return result;
}

ColorHSL colorToHSL(Color col)
{
    ColorHSL result;
    double min, max;

    max = col.r > col.g ? col.r : col.g;
    max = col.b > max ? col.b : max;

    min = col.r < col.g ? col.r : col.g;
    min = col.b < min ? col.b : min;

    result.h = result.s = result.l = (max + min) / 2.0;

    if (max == min)
    {
        result.h = result.s = 0.0;
    }
    else
    {
        double d = max - min;
        result.s = result.l > 0.5 ? d / (2.0 - max - min) : d / (max + min);
        if (max == col.r)
        {
            result.h = (col.g - col.b) / d + (col.g < col.b ? 6.0 : 0);
        }
        else if (max == col.g)
        {
            result.h = (col.b - col.r) / d + 2.0;
        }
        else
        {
            result.h = (col.r - col.g) / d + 4.0;
        }
        result.h /= 6.0;
    }

    result.a = col.a;

    return result;
}

ColorHSL colorHSLFromValues(double h, double s, double l, double a)
{
    ColorHSL result;

    result.h = h;
    result.s = s;
    result.l = l;
    result.a = a;

    return result;
}
