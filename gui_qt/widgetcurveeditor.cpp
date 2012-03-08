#include "widgetcurveeditor.h"

#include <QPainter>

WidgetCurveEditor::WidgetCurveEditor(QWidget *parent) : QWidget(parent)
{
    _curve = curveCreate();
    
    setMinimumSize(500, 500);
    setMaximumSize(500, 500);
}

WidgetCurveEditor::~WidgetCurveEditor()
{
    curveDelete(_curve);
}

void WidgetCurveEditor::paintEvent(QPaintEvent* event)
{
    double position, value;
    
    QPainter painter(this);
    painter.fillRect(0, 0, 500, 500, QColor(255, 255, 255));
    painter.setPen(QColor(255, 0, 0));
    
    for (int x = 0; x < 500; x++)
    {
        position = ((double)x) / 499.0;
        value = curveGetValue(_curve, position);
        painter.drawPoint(x, 499 - (int)(value * 499.0));
    }
}
