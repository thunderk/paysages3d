#include "WidgetPreviewCanvas.h"

#include "tools.h"
#include "Canvas.h"
#include "CanvasPreview.h"

#include <QPainter>

WidgetPreviewCanvas::WidgetPreviewCanvas(QWidget *parent) :
    QWidget(parent), canvas(NULL)
{
    pixbuf = new QImage();
    inited = false;

    startTimer(500);
}

WidgetPreviewCanvas::~WidgetPreviewCanvas()
{
    delete pixbuf;
}

void WidgetPreviewCanvas::setCanvas(const Canvas *canvas)
{
    this->canvas = canvas;
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
        if (!inited)
        {
            canvas->getPreview()->initLive(this);
            inited = true;
        }

        canvas->getPreview()->updateLive(this);
        update();
    }
}
