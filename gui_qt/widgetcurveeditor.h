#ifndef _PAYSAGES_QT_WIDGETCURVEEDITOR_H_
#define _PAYSAGES_QT_WIDGETCURVEEDITOR_H_

#include <QWidget>
#include <QColor>
#include "../lib_paysages/curve.h"

class WidgetCurveEditor : public QWidget
{
    Q_OBJECT
    
public:
    WidgetCurveEditor(QWidget* parent, double xmin, double xmax, double ymin, double ymax);
    ~WidgetCurveEditor();
    
    void setCurve(Curve* curve);
    void getCurve(Curve* curve);
    
    void setPenColor(QColor color);
    
signals:
    void liveChanged();
    
protected:
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    
private:
    int getPointAt(int x, int y);
    
    Curve* _curve;
    int _dragged;
    QColor _pen;
};

#endif
