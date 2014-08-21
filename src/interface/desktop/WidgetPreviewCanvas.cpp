#include "WidgetPreviewCanvas.h"

#include "tools.h"
#include "Canvas.h"
#include "CanvasPreview.h"

#include <QPainter>

WidgetPreviewCanvas::WidgetPreviewCanvas(QWidget *parent) :
    QWidget(parent), canvas(NULL)
{
    pixbuf = new QImage();

    startTimer(500);
}

WidgetPreviewCanvas::~WidgetPreviewCanvas()
{
    delete pixbuf;
}

void WidgetPreviewCanvas::setCanvas(const Canvas *canvas)
{
    if (not this->canvas)
    {
        this->canvas = canvas;
        canvas->getPreview()->initLive(this);
    }
}

void WidgetPreviewCanvas::setToneMapping(const ColorProfile &profile)
{
    if (canvas)
    {
        canvas->getPreview()->setToneMapping(profile);
        canvas->getPreview()->updateLive(this);
        update();
    }
}

void WidgetPreviewCanvas::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(0, 0, *pixbuf);
}

void WidgetPreviewCanvas::canvasResized(int width, int height)
{
    if (QSize(width, height) != this->size())
    {
        setMaximumSize(width, height);
        setMinimumSize(width, height);
        resize(width, height);

        delete pixbuf;
        pixbuf = new QImage(width, height, QImage::Format_ARGB32);
    }
}

void WidgetPreviewCanvas::canvasCleared(const Color &col)
{
    pixbuf->fill(colorToQColor(col));
}

void WidgetPreviewCanvas::canvasPainted(int x, int y, const Color &col)
{
    pixbuf->setPixel(x, pixbuf->height() - 1 - y, colorToQColor(col).rgb());
}

void WidgetPreviewCanvas::timerEvent(QTimerEvent *)
{
    if (canvas)
    {
        canvas->getPreview()->updateLive(this);
        update();
    }
}
