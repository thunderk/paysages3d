#include "RenderPreviewProvider.h"

#include "Canvas.h"
#include "CanvasPreview.h"
#include "Color.h"

static inline QColor colorToQColor(Color color) {
    color.normalize();
    return QColor(color.r * 255.0, color.g * 255.0, color.b * 255.0, color.a * 255.0);
}

RenderPreviewProvider::RenderPreviewProvider() : QQuickImageProvider(QQuickImageProvider::Image) {
    canvas = NULL;
    pixbuf = new QImage(1, 1, QImage::Format_ARGB32);
    hide();
}

RenderPreviewProvider::~RenderPreviewProvider() {
    delete pixbuf;
}

QImage RenderPreviewProvider::requestImage(const QString &, QSize *size, const QSize &) {
    if (canvas) {
        canvas->getPreview()->updateLive(this);
    }

    if (size) {
        *size = pixbuf->size();
    }

    return *pixbuf;
}

void RenderPreviewProvider::setCanvas(const Canvas *canvas) {
    if (not this->canvas) {
        this->canvas = canvas;
        pixbuf->fill(Qt::black);
        canvas->getPreview()->initLive(this);
    }
}

void RenderPreviewProvider::releaseCanvas() {
    if (canvas) {
        canvas->getPreview()->updateLive(this);
        canvas = NULL;
    }
}

void RenderPreviewProvider::hide() {
    pixbuf->fill(Qt::black);
}

void RenderPreviewProvider::setToneMapping(const ColorProfile &profile) {
    if (canvas) {
        canvas->getPreview()->setToneMapping(profile);
        canvas->getPreview()->updateLive(this);
    }
}

void RenderPreviewProvider::canvasResized(int width, int height) {
    if (QSize(width, height) != pixbuf->size()) {
        delete pixbuf;
        pixbuf = new QImage(width, height, QImage::Format_ARGB32);
    }
    pixbuf->fill(Qt::black);
}

void RenderPreviewProvider::canvasCleared(const Color &col) {
    pixbuf->fill(colorToQColor(col));
}

void RenderPreviewProvider::canvasPainted(int x, int y, const Color &col) {
    pixbuf->setPixel(x, pixbuf->height() - 1 - y, colorToQColor(col).rgb());
}
