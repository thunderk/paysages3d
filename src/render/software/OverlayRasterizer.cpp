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
    double width = (double)renderer->render_camera->getWidth();
    double height = (double)renderer->render_camera->getHeight();
    Vector3 topleft = renderer->unprojectPoint(Vector3(height, 0.0, 1.0));
    Vector3 bottomleft = renderer->unprojectPoint(Vector3(0.0, 0.0, 1.0));
    Vector3 topright = renderer->unprojectPoint(Vector3(height, width, 1.0));
    Vector3 bottomright = renderer->unprojectPoint(Vector3(0.0, width, 1.0));

    pushQuad(canvas, topleft, bottomleft, bottomright, topright);
}

Color OverlayRasterizer::shadeFragment(const CanvasFragment &fragment, const CanvasFragment *) const {
    double width = (double)renderer->render_camera->getWidth() - 1.0;
    double height = (double)renderer->render_camera->getHeight() - 1.0;
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

    return processPixel((int)x, (int)y, relx, rely);
}
