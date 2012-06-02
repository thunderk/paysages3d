#include "formmaterial.h"

#include "../lib_paysages/lighting.h"

/**************** Previews ****************/
class PreviewMaterialColor:public BasePreview
{
public:
    PreviewMaterialColor(QWidget* parent, SurfaceMaterial* material) : BasePreview(parent)
    {
        configScaling(0.05, 2.0, 0.05, 2.0);
    }
protected:
    /*void updateData()
    {
        noiseCopy(_noise_original, _noise_preview);
    }*/
    QColor getColor(double x, double y)
    {
        return QColor(0, 0, 0);
    }
private:
    SurfaceMaterial* _material;
};

/**************** Form ****************/
FormMaterial::FormMaterial(QWidget* parent, SurfaceMaterial* material) : BaseForm(parent)
{
    _initial = material;
    _material = *_initial;
    
    addInputColor(tr("Base color"), &_material.base);
    addInputDouble(tr("Light reflection"), &_material.reflection, 0.0, 1.0, 0.01, 0.1);
    addInputDouble(tr("Light reflection shininess"), &_material.shininess, 0.0, 20.0, 0.1, 1.0);
    
    _preview_color = new PreviewMaterialColor(this, &_material);
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
