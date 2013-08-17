#ifndef _PAYSAGES_QT_PREVIEWMATERIAL_H_
#define _PAYSAGES_QT_PREVIEWMATERIAL_H_

#include "basepreview.h"

#include "rendering/tools/lighting.h"
#include "rendering/renderer.h"

class SmallMaterialPreview:public QWidget
{
public:
    SmallMaterialPreview(QWidget* parent, SurfaceMaterial* material);
    ~SmallMaterialPreview();

    Color getColor(double x, double y);

protected:
    virtual void paintEvent(QPaintEvent* event);

private:
    SurfaceMaterial* _material;
    LightDefinition _light;
    Renderer* _renderer;
    ColorProfile* _color_profile;
};

class PreviewMaterial:public BasePreview
{
    Q_OBJECT

public:
    PreviewMaterial(QWidget* parent, SurfaceMaterial* material);
    ~PreviewMaterial();

protected:
    virtual Color getColor(double x, double y);

private:
    SmallMaterialPreview* _small;
};

#endif
