#ifndef _PAYSAGES_QT_INPUTLAYERS_H_
#define _PAYSAGES_QT_INPUTLAYERS_H_

#include <QWidget>
#include "baseinput.h"
#include "dialoglayers.h"
#include "Layers.h"

class InputLayers:public BaseInput
{
    Q_OBJECT

public:
    InputLayers(QWidget* form, QString label, Layers* value, FormLayerBuilder form_builder);

public slots:
    virtual void updatePreview();
    void editLayers();

private:
    Layers* _value;
    FormLayerBuilder _form_builder;
};

#endif
