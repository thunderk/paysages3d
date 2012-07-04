#ifndef _PAYSAGES_QT_INPUTDOUBLE_H_
#define _PAYSAGES_QT_INPUTDOUBLE_H_

#include <QWidget>
#include <QSlider>
#include "baseinput.h"

class InputDouble:public BaseInput
{
    Q_OBJECT

public:
    InputDouble(QWidget* form, QString label, double* value, double min, double max, double small_step, double large_step);

public slots:
    virtual void updatePreview();
    virtual void applyValue();
    virtual void revert();

private:
    QSlider* _slider;
    double* _value;
    double _min;
    double _max;
    double _small_step;
    double _large_step;
};

#endif
