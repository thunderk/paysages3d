#include "AtmosphereResult.h"

AtmosphereResult::AtmosphereResult()
{
    base = COLOR_BLACK;
    inscattering = COLOR_BLACK;
    attenuation = COLOR_WHITE;
    mask = COLOR_TRANSPARENT;
    distance = 0.0;
    final = COLOR_BLACK;
}

void AtmosphereResult::updateFinal()
{
    final.r = base.r * attenuation.r + inscattering.r;
    final.g = base.g * attenuation.g + inscattering.g;
    final.b = base.b * attenuation.b + inscattering.b;
    final.a = 1.0;

    final.mask(mask);
}
