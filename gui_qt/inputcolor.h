#ifndef _GUI_QT_INPUTCOLOR_H_
#define _GUI_QT_INPUTCOLOR_H_

#include <QWidget>
#include "baseinput.h"

#include "../lib_paysages/shared/types.h"

class InputColor:public BaseInput
{
    Q_OBJECT

public:
    InputColor(QWidget* form, QString label, Color value);

public slots:
    virtual void revert();

protected slots:
    virtual void applyValue();

private:
    Color _color;
};

#endif // _GUI_QT_INPUTCOLOR_H_
