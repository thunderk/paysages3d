#ifndef _PAYSAGES_QT_WIDGETCURVEEDITOR_H_
#define _PAYSAGES_QT_WIDGETCURVEEDITOR_H_

#include <QWidget>
#include <QColor>
#include "rendering/tools/curve.h"

class WidgetCurveEditor : public QWidget
{
    Q_OBJECT

public:
    WidgetCurveEditor(QWidget* parent, double xmin, double xmax, double ymin, double ymax);
    ~WidgetCurveEditor();

    void setAxisLabels(QString xlabel, QString ylabel);

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
    void curveToScreen(double curve_x, double curve_y, int* screen_x, int* screen_y);
    void screenToCurve(int screen_x, int screen_y, double* curve_x, double* curve_y);
    int getPointAt(int x, int y);

    double _xmin;
    double _xmax;
    double _ymin;
    double _ymax;
    Curve* _curve;
    int _dragged;
    QColor _pen;
    QString _xlabel;
    QString _ylabel;
};

#endif
