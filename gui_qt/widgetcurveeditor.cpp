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

void WidgetCurveEditor::setCurve(Curve* curve)
{
    curveCopy(curve, _curve);
    update();
}

void WidgetCurveEditor::getCurve(Curve* curve)
{
    curveCopy(_curve, curve);
}

void WidgetCurveEditor::paintEvent(QPaintEvent* event)
{
    int i, n;
    CurvePoint point;
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
    
    n = curveGetPointCount(_curve);
    for (i = 0; i < n; i++)
    {
        curveGetPoint(_curve, i, &point);
        painter.drawEllipse(QPoint((int)(point.position * 499.0), 499 - (int)(point.value * 499.0)), 4.0, 4.0);
    }
}
