#include "widgetcurveeditor.h"

#include <QPainter>
#include <QMouseEvent>
#include "../lib_paysages/tools.h"

WidgetCurveEditor::WidgetCurveEditor(QWidget *parent) : QWidget(parent)
{
    _curve = curveCreate();
    _dragged = -1;
    
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
        painter.drawEllipse(QPointF((int)(point.position * 499.0), 499 - (int)(point.value * 499.0)), 4.0, 4.0);
    }
}

void WidgetCurveEditor::mousePressEvent(QMouseEvent* event)
{
    int i, n;
    int nearest;
    double mousex, mousey;
    double distance, ndistance;
    CurvePoint point;

    n = curveGetPointCount(_curve);
    if (n < 1)
    {
        return;
    }

    mousex = ((double)event->x()) / 499.0;
    mousey = 1.0 - ((double)event->y()) / 499.0;
    nearest = -1;
    
    // Find nearest point
    for (i = 0; i < n; i++)
    {
        curveGetPoint(_curve, i, &point);
        ndistance = toolsGetDistance2D(point.position, point.value, mousex, mousey);
        if (nearest < 0 || ndistance < distance)
        {
            distance = ndistance;
            nearest = i;
        }
    }
    
    if (distance < 0.015)
    {
        _dragged = nearest;
    }
    else
    {
        _dragged = -1;
    }
    
    event->accept();
}

void WidgetCurveEditor::mouseMoveEvent(QMouseEvent* event)
{
    CurvePoint point;
    
    if (_dragged >= 0)
    {
        point.position = ((double)event->x()) / 499.0;
        point.value = 1.0 - ((double)event->y()) / 499.0;
        
        point.position = (point.position < 0.0) ? 0.0 : point.position;
        point.position = (point.position > 1.0) ? 1.0 : point.position;
        point.value = (point.value < 0.0) ? 0.0 : point.value;
        point.value = (point.value > 1.0) ? 1.0 : point.value;
        
        curveSetPoint(_curve, _dragged, &point);

        update();
        
        emit liveChanged();
    }
    
    event->accept();
}

void WidgetCurveEditor::mouseReleaseEvent(QMouseEvent* event)
{
    if (_dragged >= 0)
    {
        _dragged = -1;
        curveValidate(_curve);
        
        update();
    }
    
    event->accept();
}
