#include "private.h"

#include "../tools/euclid.h"
#include "../renderer.h"
#include "../tools.h"

/*
 * Clouds previews.
 */

Color _fakeApplyLightingToSurface(Renderer* renderer, Vector3 location, Vector3 normal, SurfaceMaterial* material)
{
    UNUSED(renderer);
    UNUSED(location);
    UNUSED(normal);
    UNUSED(material);

    return COLOR_WHITE;
}

Renderer* cloudsCreatePreviewCoverageRenderer()
{
    Renderer* result = rendererCreate();
    result->render_quality = 5;
    result->applyLightingToSurface = _fakeApplyLightingToSurface;
    return result;
}

Color cloudsGetPreviewCoverage(Renderer* renderer, double x, double y, double scaling)
{
    Vector3 eye, look;

    eye.x = 0.0;
    eye.y = scaling;
    eye.z = -10.0 * scaling;
    look.x = x * 0.01 / scaling;
    look.y = -y * 0.01 / scaling - 0.3;
    look.z = 1.0;
    look = v3Normalize(look);

    return renderer->clouds->getColor(renderer, COLOR_BLUE, eye, v3Add(eye, v3Scale(look, 1000.0)));
}

Renderer* cloudsCreatePreviewColorRenderer()
{
    Renderer* result = rendererCreate();
    result->render_quality = 8;
    return result;
}

Color cloudsGetPreviewColor(Renderer* renderer, double x, double y)
{
    Vector3 start, end;
    double thickness = 0.5;

    start.x = x * thickness * 0.5;
    start.y = -y * thickness * 0.5;
    start.z = thickness * 0.5;

    end.x = start.x;
    end.y = start.y;
    end.z = -start.z;

    return renderer->clouds->getColor(renderer, COLOR_BLUE, start, end);
}
