#include "previewterrainshape.h"

PreviewTerrainShape::PreviewTerrainShape(QWidget *parent) :
    BasePreview(parent)
{
    _renderer = terrainCreatePreviewRenderer();

    addOsd(QString("geolocation"));

    configScaling(20.0, 1000.0, 20.0, 50.0);
    configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
}

Color PreviewTerrainShape::getColor(double x, double y)
{
    return terrainGetPreviewColor(_renderer, x, y, scaling);
}

void PreviewTerrainShape::updateData()
{
    //TerrainRendererClass.bind(_renderer, _definition);
}
