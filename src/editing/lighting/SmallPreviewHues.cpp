#include "SmallPreviewHues.h"

#include <QPainter>

#include "editing/tools.h"

SmallPreviewHues::SmallPreviewHues(QWidget* parent) : DrawingWidget(parent)
{
}

void SmallPreviewHues::doDrawing(QPainter* painter)
{
    int x;
    ColorHSL colhsl;

    colhsl.s = 1.0;
    colhsl.l = 0.5;
    colhsl.a = 1.0;

    for (x = 0; x < width(); x++)
    {
        colhsl.h = (double)x / (double)(width() - 1);
        painter->setPen(colorToQColor(colorFromHSL(colhsl)));
        painter->drawLine(x, 0, x, height() - 1);
    }
}
