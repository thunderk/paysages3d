#ifndef _PAYSAGES_QT_INPUTBOOLEAN_H_
#define _PAYSAGES_QT_INPUTBOOLEAN_H_

#include <QWidget>
#include <QCheckBox>
#include "baseinput.h"

class InputBoolean:public BaseInput
{
    Q_OBJECT

public:
    InputBoolean(QWidget* form, QString label, int* value);

public slots:
    virtual void updatePreview();
    virtual void applyValue();
    virtual void revert();

private:
    QCheckBox* checkbox;
    int* value;
};

#endif
