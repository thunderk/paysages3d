#include "color.h"

#include <cstdlib>
#include <cassert>
#include <cstring>
#include <cmath>
#include "PackStream.h"
#include "Curve.h"

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
