#ifndef _PAYSAGES_QT_INPUTENUM_H_
#define _PAYSAGES_QT_INPUTENUM_H_

#include <QWidget>
#include <QStringList>
#include "baseinput.h"

class InputEnum:public BaseInput
{
    Q_OBJECT

public:
    InputEnum(QWidget* form, QString label, int* value, const QStringList& values);

public slots:
    virtual void applyValue();
    virtual void revert();

private:
    int* _value;
};

#endif
