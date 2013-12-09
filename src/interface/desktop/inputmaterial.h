#ifndef _PAYSAGES_QT_INPUTMATERIAL_H_
#define _PAYSAGES_QT_INPUTMATERIAL_H_

#include <QWidget>
#include "baseinput.h"

class InputMaterial:public BaseInput
{
    Q_OBJECT

public:
    InputMaterial(QWidget* form, QString label, SurfaceMaterial* value);

public slots:
    virtual void updatePreview();
    virtual void applyValue();
    virtual void revert();

private slots:
    void editMaterial();

private:
    SurfaceMaterial* _value;
};

#endif
