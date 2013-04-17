#ifndef _PAYSAGES_QT_FORMMATERIAL_H_
#define _PAYSAGES_QT_FORMMATERIAL_H_

#include "basepreview.h"
#include "baseform.h"

#include "rendering/shared/types.h"

class FormMaterial : public BaseForm
{
    Q_OBJECT

public:
    FormMaterial(QWidget* parent, SurfaceMaterial* material);
    void getMaterial(SurfaceMaterial* material);

public slots:
    virtual void revertConfig();
//    virtual void applyConfig();

private:
    BasePreview* _preview_color;
    SurfaceMaterial* _initial;
    SurfaceMaterial _material;
};

#endif
