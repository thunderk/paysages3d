#include "private.h"

#include "../terrain/public.h"
#include "../renderer.h"
#include "../tools.h"

void waterAlterPreviewCoverageRenderer(Renderer* renderer)
{
    terrainAlterPreviewRenderer(renderer);
}

Color waterGetPreviewCoverage(Renderer* renderer, double x, double y, double scaling, int highlight_enabled)
{
    double height;

    height = renderer->terrain->getHeight(renderer, x, y, 1);
    if (height > renderer->terrain->getWaterHeight(renderer))
    {
        return terrainGetPreviewColor(renderer, x, y, scaling);
    }
    else
    {
        Color base;

        base = renderer->water->getResult(renderer, x, y).final;

        if (highlight_enabled)
        {
            Color mask = {0.5, 0.5, 1.0, 0.5};
            colorMask(&base, &mask);
        }

        return base;
    }
}

/*Renderer* waterCreatePreviewColorRenderer()
{
}

Color waterGetPreviewColor(Renderer* renderer, double x, double y, double scaling)
{

}*/
