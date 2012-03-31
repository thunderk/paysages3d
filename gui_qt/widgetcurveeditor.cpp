#include "widgetcurveeditor.h"

#include <QPainter>
#include <QMouseEvent>
#include "../lib_paysages/tools.h"

WidgetCurveEditor::WidgetCurveEditor(QWidget *parent) : QWidget(parent)
{
    _curve = curveCreate();
    _dragged = -1;
    _pen = QColor(0, 0, 0);
    
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

void WidgetCurveEditor::setPenColor(QColor color)
{
    _pen = color;
    update();
}

void WidgetCurveEditor::paintEvent(QPaintEvent* event)
{
    int i, n;
    CurvePoint point;
    double position, value;
    
    QPainter painter(this);
    painter.fillRect(0, 0, 500, 500, QColor(255, 255, 255));
    painter.setPen(_pen);
    
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
    if (event->button() == Qt::LeftButton && _dragged < 0)
    {
        _dragged = getPointAt(event->x(), event->y());
    }
    
    event->accept();
}

void WidgetCurveEditor::mouseMoveEvent(QMouseEvent* event)
{
    CurvePoint point;
    
    if (_dragged >= 0 && (event->buttons() & Qt::LeftButton))
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
    int clicked;
    
    if (event->button() == Qt::RightButton)
    {
        clicked = getPointAt(event->x(), event->y());
        if (clicked >= 0)
        {
            curveRemovePoint(_curve, clicked);
            update();
            emit liveChanged();
        }
    }
    else if (event->button() == Qt::LeftButton && _dragged >= 0)
    {
        _dragged = -1;
        curveValidate(_curve);
        update();
    }
    
    event->accept();
}

void WidgetCurveEditor::mouseDoubleClickEvent(QMouseEvent* event)
{
    CurvePoint point;
    
    if (event->button() == Qt::LeftButton && _dragged < 0)
    {
        if (getPointAt(event->x(), event->y()) < 0)
        {
            point.position = ((double)event->x()) / 499.0;
            point.value = 1.0 - ((double)event->y()) / 499.0;

            curveAddPoint(_curve, &point);
            curveValidate(_curve);
            update();
            emit liveChanged();
        }
    }
}

int WidgetCurveEditor::getPointAt(int x, int y)
{
    int n;
    int nearest;
    double distance, ndistance;
    CurvePoint point;
    double dx = ((double)x) / 499.0;
    double dy = 1.0 - ((double)y) / 499.0;

    n = curveGetPointCount(_curve);
    if (n < 1)
    {
        return -1;
    }

    // Find nearest point
    nearest = -1;
    for (int i = 0; i < n; i++)
    {
        curveGetPoint(_curve, i, &point);
        ndistance = toolsGetDistance2D(point.position, point.value, dx, dy);
        if (nearest < 0 || ndistance < distance)
        {
            distance = ndistance;
            nearest = i;
        }
    }
    
    if (nearest >= 0 && distance < 0.015)
    {
        return nearest;
    }
    else
    {
        return -1;
    }
}
