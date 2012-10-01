#ifndef _PAYSAGES_QT_INPUTINT_H_
#define _PAYSAGES_QT_INPUTINT_H_

#include <QWidget>
#include <QSlider>
#include "inputdouble.h"

class InputInt:public InputDouble
{
    Q_OBJECT

public:
    InputInt(QWidget* form, QString label, int* value, int min, int max, int small_step, int large_step);

public slots:
    virtual void updatePreview();
    virtual void applyValue();
    virtual void revert();

private:
    int* _value;
    double _dvalue;
};

#endif
