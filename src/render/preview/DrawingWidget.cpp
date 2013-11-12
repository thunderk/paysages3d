#include "DrawingWidget.h"

#include <QPainter>
#include <QPainterPath>

DrawingWidget::DrawingWidget(QWidget* parent) :
QWidget(parent)
{
}

void DrawingWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    QPainterPath mask;
    mask.addRoundedRect(rect(), 8.0, 8.0);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setClipPath(mask);
    painter.setRenderHint(QPainter::Antialiasing, false);

    doDrawing(&painter);
}
