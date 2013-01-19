#ifndef _PAYSAGES_QT_INPUTCURVE_H_
#define _PAYSAGES_QT_INPUTCURVE_H_

#include <QWidget>
#include "baseinput.h"

#include "../lib_paysages/tools/curve.h"

class InputCurve:public BaseInput
{
    Q_OBJECT

public:
    InputCurve(QWidget* form, QString label, Curve* value, double xmin, double xmax, double ymin, double ymax, QString xlabel, QString ylabel);

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
    QString _xlabel;
    QString _ylabel;
};

#endif
