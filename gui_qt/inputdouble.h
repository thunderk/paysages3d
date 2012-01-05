#ifndef _GUI_QT_INPUTDOUBLE_H_
#define _GUI_QT_INPUTDOUBLE_H_

#include <QWidget>
#include <QSlider>
#include "baseinput.h"

class InputDouble:public BaseInput
{
    Q_OBJECT

public:
    InputDouble(QWidget* form, QString label, double* value, double min, double max, double small_step, double large_step);

public slots:
    virtual void revert();

protected slots:
    virtual void applyValue();

private:
    QSlider* slider;
    double* value;
    double min;
    double max;
    double small_step;
    double large_step;
};

#endif // _GUI_QT_INPUTDOUBLE_H_
