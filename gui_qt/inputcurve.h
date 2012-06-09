#ifndef _PAYSAGES_QT_INPUTCURVE_H_
#define _PAYSAGES_QT_INPUTCURVE_H_

#include <QWidget>
#include "baseinput.h"

#include "../lib_paysages/curve.h"

class InputCurve:public BaseInput
{
    Q_OBJECT

public:
    InputCurve(QWidget* form, QString label, Curve* value, double xmin, double xmax, double ymin, double ymax);

public slots:
    virtual void updatePreview();
    virtual void applyValue();
    virtual void revert();

private slots:
    void editCurve();

private:
    Curve* _value;
    double _xmin;
    double _xmax;
    double _ymin;
    double _ymax;
};

#endif