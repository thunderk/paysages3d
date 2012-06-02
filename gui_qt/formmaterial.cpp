#include "formmaterial.h"

#include "previewmaterial.h"

/**************** Form ****************/
FormMaterial::FormMaterial(QWidget* parent, SurfaceMaterial* material) : BaseForm(parent)
{
    _initial = material;
    _material = *_initial;
    
    addInputColor(tr("Base color"), &_material.base);
    addInputDouble(tr("Light reflection"), &_material.reflection, 0.0, 1.0, 0.01, 0.1);
    addInputDouble(tr("Light reflection shininess"), &_material.shininess, 0.0, 20.0, 0.1, 1.0);
    
    _preview_color = new PreviewMaterial(this, &_material);
    addPreview(_preview_color, tr("Lighting preview"));

    revertConfig();
}

void FormMaterial::getMaterial(SurfaceMaterial* material)
{
    *material = _material;
}

void FormMaterial::revertConfig()
{
    _material = *_initial;
    
    BaseForm::revertConfig();
}
