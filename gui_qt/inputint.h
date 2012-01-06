#ifndef _PAYSAGES_QT_INPUTINT_H_
#define _PAYSAGES_QT_INPUTINT_H_

#include <QWidget>
#include <QSlider>
#include "baseinput.h"

class InputInt:public BaseInput
{
    Q_OBJECT

public:
    InputInt(QWidget* form, QString label, int* value, int min, int max, int small_step, int large_step);

public slots:
    virtual void revert();

protected slots:
    virtual void applyValue();

private:
    QSlider* slider;
    int* value;
    int min;
    int max;
    int small_step;
    int large_step;
};

#endif // _PAYSAGES_QT_INPUTINT_H_
