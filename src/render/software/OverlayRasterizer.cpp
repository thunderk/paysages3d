#include "OverlayRasterizer.h"

#include <cmath>
#include "Color.h"
#include "SoftwareRenderer.h"
#include "CameraDefinition.h"
#include "CanvasFragment.h"

OverlayRasterizer::OverlayRasterizer(SoftwareRenderer *renderer, RenderProgress *progress)
    : Rasterizer(renderer, progress, 0, COLOR_WHITE) {
}

int OverlayRasterizer::prepareRasterization() {
    setPerspectiveCorrection(false);
    return 1;
}

void OverlayRasterizer::rasterizeToCanvas(CanvasPortion *canvas) {
    double width = to_double(renderer->render_camera->getWidth());
    double height = to_double(renderer->render_camera->getHeight());
    Vector3 topleft = renderer->unprojectPoint(Vector3(0.0, height, 1.0001));
    Vector3 bottomleft = renderer->unprojectPoint(Vector3(0.0, 0.0, 1.0001));
    Vector3 topright = renderer->unprojectPoint(Vector3(width, height, 1.0001));
    Vector3 bottomright = renderer->unprojectPoint(Vector3(width, 0.0, 1.0001));

    pushQuad(canvas, topleft, bottomleft, bottomright, topright);
}

Color OverlayRasterizer::shadeFragment(const CanvasFragment &fragment, const CanvasFragment *) const {
    double width = to_double(renderer->render_camera->getWidth() - 1.0);
    double height = to_double(renderer->render_camera->getHeight() - 1.0);
    double relx;
    double rely;
    double x = floor(fragment.getPixel().x);
    double y = floor(fragment.getPixel().y);

    if (width > height) {
        relx = 2.0 * ((x - (width - height) * 0.5) / height - 0.5);
        rely = 2.0 * (y / height - 0.5);
    } else {
        relx = 2.0 * (x / height - 0.5);
        rely = 2.0 * ((y - (height - width) * 0.5) / height - 0.5);
    }

    return processPixel(trunc_to_int(x), trunc_to_int(y), relx, rely);
}
