#ifndef _PAYSAGES_QT_INPUTLAYERS_H_
#define _PAYSAGES_QT_INPUTLAYERS_H_

#include <QWidget>
#include "baseinput.h"
#include "../lib_paysages/layers.h"

class InputLayers:public BaseInput
{
    Q_OBJECT

public:
    InputLayers(QWidget* form, QString label, Layers* value);

public slots:
    virtual void updatePreview();

private:
    Layers* _value;
};

#endif
