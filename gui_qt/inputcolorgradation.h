#ifndef _PAYSAGES_QT_INPUTCOLORGRADATION_H_
#define _PAYSAGES_QT_INPUTCOLORGRADATION_H_

#include <QWidget>
#include "baseinput.h"

#include "../lib_paysages/shared/types.h"

class InputColorGradation:public BaseInput
{
    Q_OBJECT

public:
    InputColorGradation(QWidget* form, QString label, ColorGradation* value);

public slots:
    virtual void revert();

protected slots:
    virtual void applyValue();

private slots:
    void editGradation();

private:
    ColorGradation* _value;
};

#endif // _PAYSAGES_QT_INPUTCOLORGRADATION_H_
