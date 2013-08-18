#ifndef _PAYSAGES_QT_PREVIEWMATERIAL_H_
#define _PAYSAGES_QT_PREVIEWMATERIAL_H_

#include "basepreview.h"
#include "editing/common/previewrenderer.h"

#include "rendering/tools/lighting.h"
#include "rendering/renderer.h"

class MaterialPreviewRenderer:public PreviewRenderer {
    Q_OBJECT
public:
    MaterialPreviewRenderer(SurfaceMaterial* material);
    ~MaterialPreviewRenderer();

    virtual void bindEvent(BasePreview* preview);
    virtual Color getColor2D(double x, double y, double scaling);

private:
    SurfaceMaterial* _material;
    LightDefinition _light;
    ColorProfile* _color_profile;
};

class SmallMaterialPreview:public QWidget
{
public:
    SmallMaterialPreview(QWidget* parent, SurfaceMaterial* material);

    Color getColor(double x, double y);

protected:
    virtual void paintEvent(QPaintEvent* event);

private:
    MaterialPreviewRenderer _renderer;
};

#endif
