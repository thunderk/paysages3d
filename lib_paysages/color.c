#include "color.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "tools.h"
#include "curve.h"

Color COLOR_TRANSPARENT = {0.0, 0.0, 0.0, 0.0};
Color COLOR_BLACK = {0.0, 0.0, 0.0, 1.0};
Color COLOR_RED = {1.0, 0.0, 0.0, 1.0};
Color COLOR_GREEN = {0.0, 1.0, 0.0, 1.0};
Color COLOR_BLUE = {0.0, 0.0, 1.0, 1.0};
Color COLOR_WHITE = {1.0, 1.0, 1.0, 1.0};
Color COLOR_GREY = {0.5, 0.5, 0.5, 1.0};

struct ColorGradation
{
    Curve* red;
    Curve* green;
    Curve* blue;
};

void colorSave(PackStream* stream, Color* col)
{
    packWriteDouble(stream, &col->r);
    packWriteDouble(stream, &col->g);
    packWriteDouble(stream, &col->b);
    packWriteDouble(stream, &col->a);
}

void colorLoad(PackStream* stream, Color* col)
{
    packReadDouble(stream, &col->r);
    packReadDouble(stream, &col->g);
    packReadDouble(stream, &col->b);
    packReadDouble(stream, &col->a);
}

unsigned int colorTo32BitRGBA(Color* col)
{
    return (((unsigned int)(col->a * 255.0)) << 24) | (((unsigned int)(col->b * 255.0)) << 16) | (((unsigned int)(col->g * 255.0)) << 8) | ((unsigned int)(col->r * 255.0));
}

unsigned int colorTo32BitBGRA(Color* col)
{
    return (((unsigned int)(col->a * 255.0)) << 24) | (((unsigned int)(col->r * 255.0)) << 16) | (((unsigned int)(col->g * 255.0)) << 8) | ((unsigned int)(col->b * 255.0));
}

unsigned int colorTo32BitARGB(Color* col)
{
    return (((unsigned int)(col->b * 255.0)) << 24) | (((unsigned int)(col->g * 255.0)) << 16) | (((unsigned int)(col->r * 255.0)) << 8) | ((unsigned int)(col->a * 255.0));
}

unsigned int colorTo32BitABGR(Color* col)
{
    return (((unsigned int)(col->r * 255.0)) << 24) | (((unsigned int)(col->g * 255.0)) << 16) | (((unsigned int)(col->b * 255.0)) << 8) | ((unsigned int)(col->a * 255.0));
}

Color colorFrom32BitRGBA(unsigned int col)
{
    Color result;
    
    result.r = ((double)(col & 0x000000FF)) / 255.0;
    result.g = ((double)((col & 0x0000FF00) >> 8)) / 255.0;
    result.b = ((double)((col & 0x00FF0000) >> 16)) / 255.0;
    result.a = ((double)((col & 0xFF000000) >> 24)) / 255.0;
    
    return result;
}

Color colorFrom32BitBGRA(unsigned int col)
{
    Color result;
    
    result.b = ((double)(col & 0x000000FF)) / 255.0;
    result.g = ((double)((col & 0x0000FF00) >> 8)) / 255.0;
    result.r = ((double)((col & 0x00FF0000) >> 16)) / 255.0;
    result.a = ((double)((col & 0xFF000000) >> 24)) / 255.0;
    
    return result;
}

Color colorFrom32BitARGB(unsigned int col)
{
    Color result;
    
    result.a = ((double)(col & 0x000000FF)) / 255.0;
    result.r = ((double)((col & 0x0000FF00) >> 8)) / 255.0;
    result.g = ((double)((col & 0x00FF0000) >> 16)) / 255.0;
    result.b = ((double)((col & 0xFF000000) >> 24)) / 255.0;
    
    return result;
}

Color colorFrom32BitABGR(unsigned int col)
{
    Color result;
    
    result.a = ((double)(col & 0x000000FF)) / 255.0;
    result.b = ((double)((col & 0x0000FF00) >> 8)) / 255.0;
    result.g = ((double)((col & 0x00FF0000) >> 16)) / 255.0;
    result.r = ((double)((col & 0xFF000000) >> 24)) / 255.0;
    
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
    double max = colorGetValue(col);

    assert(col->r >= 0.0);
    assert(col->g >= 0.0);
    assert(col->b >= 0.0);
    assert(col->a >= 0.0);

    if (max > 1.0)
    {
        col->r /= max;
        col->g /= max;
        col->b /= max;
    }
    return max;
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

ColorGradation* colorGradationCreate()
{
    ColorGradation* result;

    result = malloc(sizeof(ColorGradation));
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
    free(gradation);
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

