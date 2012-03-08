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
    
protected:
    void paintEvent(QPaintEvent* event);
    
private:
    Curve* _curve;
};

#endif
