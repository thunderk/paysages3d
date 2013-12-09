#ifndef _PAYSAGES_QT_PREVIEWMATERIAL_H_
#define _PAYSAGES_QT_PREVIEWMATERIAL_H_

#include "desktop_global.h"

#include "Base2dPreviewRenderer.h"
#include <QWidget>

class MaterialPreviewRenderer:public Base2dPreviewRenderer {
public:
    MaterialPreviewRenderer(SurfaceMaterial* material);
    virtual ~MaterialPreviewRenderer();

    virtual void bindEvent(BasePreview* preview);
    virtual Color getColor2D(double x, double y, double scaling);

private:
    SurfaceMaterial* _material;
    LightComponent* _light;
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
