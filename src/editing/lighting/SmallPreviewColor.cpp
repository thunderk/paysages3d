#include "SmallPreviewColor.h"

#include <QPainter>

#include "editing/tools.h"

SmallPreviewColor::SmallPreviewColor(QWidget* parent, Color* color) : DrawingWidget(parent)
{
    _color = color;
}

void SmallPreviewColor::setColor(Color* color)
{
    _color = color;
    update();
}

void SmallPreviewColor::doDrawing(QPainter* painter)
{
    if (_color)
    {
        painter->fillRect(rect(), colorToQColor(*_color));
    }
}
