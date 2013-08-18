#include "SmallPreviewHues.h"

#include <QPainter>

#include "editing/tools.h"

SmallPreviewHues::SmallPreviewHues(QWidget* parent) : DrawingWidget(parent)
{
}

void SmallPreviewHues::doDrawing(QPainter* painter)
{
    painter->fillRect(rect(), Qt::blue);
}
