#include "WidgetPreviewCanvas.h"

#include "Canvas.h"

WidgetPreviewCanvas::WidgetPreviewCanvas(QWidget *parent) :
    QWidget(parent), canvas(NULL)
{
    startTimer(1000);
}

void WidgetPreviewCanvas::setCanvas(const Canvas *canvas)
{
    this->canvas = canvas;
}

void WidgetPreviewCanvas::canvasResized(int width, int height)
{
    // TODO
}

void WidgetPreviewCanvas::canvasCleared(const Color &col)
{
    // TODO
}

void WidgetPreviewCanvas::canvasPainted(int x, int y, const Color &col)
{
    // TODO
}

void WidgetPreviewCanvas::timerEvent(QTimerEvent *)
{
    // Refresh the view
    if (canvas)
    {
        int width = canvas->getPreviewWidth();
        int height = canvas->getPreviewHeight();

        if (QSize(width, height) != this->size())
        {
            setMaximumSize(width, height);
            setMinimumSize(width, height);
            resize(width, height);
        }
    }
}
