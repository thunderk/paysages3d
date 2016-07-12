#pragma once

#include "basics_global.h"

typedef struct {
    double h;
    double s;
    double l;
    double a;
} ColorHSL;

BASICSSHARED_EXPORT Color colorFromHSL(const ColorHSL &col);
BASICSSHARED_EXPORT ColorHSL colorToHSL(const Color &col);

BASICSSHARED_EXPORT ColorHSL colorHSLFromValues(double h, double s, double l, double a);
