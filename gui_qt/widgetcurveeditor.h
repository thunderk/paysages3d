#ifndef _PAYSAGES_QT_WIDGETCURVEEDITOR_H_
#define _PAYSAGES_QT_WIDGETCURVEEDITOR_H_

#include <QWidget>
#include "../lib_paysages/curve.h"

class WidgetCurveEditor : public QWidget
{
    Q_OBJECT
    
public:
    WidgetCurveEditor(QWidget* parent);
    ~WidgetCurveEditor();
    
    void setCurve(Curve* curve);
    void getCurve(Curve* curve);
    
signals:
    void liveChanged();
    
protected:
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    
private:
    Curve* _curve;
    int _dragged;
};

#endif
