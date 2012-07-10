#ifndef _PAYSAGES_QT_INPUTCOLOR_H_
#define _PAYSAGES_QT_INPUTCOLOR_H_

#include <QWidget>
#include "baseinput.h"

#include "../lib_paysages/shared/types.h"

class InputColor:public BaseInput
{
    Q_OBJECT

public:
    InputColor(QWidget* form, QString label, Color* value);

public slots:
    virtual void updatePreview();
    virtual void applyValue();
    virtual void revert();

private slots:
    void chooseColor();

private:
    Color* _value;
};

#endif
