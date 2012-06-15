#ifndef _PAYSAGES_QT_DIALOGCURVE_H_
#define _PAYSAGES_QT_DIALOGCURVE_H_

#include <QDialog>
#include <QPushButton>
#include "widgetcurveeditor.h"

#include "../lib_paysages/curve.h"

class DialogCurve : public QDialog
{
    Q_OBJECT
public:
    explicit DialogCurve(QWidget* parent, Curve* curve, float xmin, float xmax, float ymin, float ymax);
    ~DialogCurve();

    static bool getCurve(QWidget* parent, Curve* curve, float xmin, float xmax, float ymin, float ymax);

public slots:
    virtual void accept();
    void revert();

protected:
    virtual void closeEvent(QCloseEvent* e);

private:
    void revertToCurrent();

private:
    Curve* _base;
    Curve* _current;
    WidgetCurveEditor* _curve_editor;
    QPushButton* _button_accept;
    QPushButton* _button_revert;
    QPushButton* _button_cancel;
};

#endif
