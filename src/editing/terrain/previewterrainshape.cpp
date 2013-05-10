#include "previewterrainshape.h"

#include "basepreview.h"

PreviewTerrainShape::PreviewTerrainShape(TerrainDefinition* terrain)
{
    _terrain = terrain;

    // TODO Don't delete the base renderer, just alter it
    rendererDelete(renderer);
    renderer = terrainCreatePreviewRenderer();
}

void PreviewTerrainShape::bindEvent(BasePreview* preview)
{
    preview->addOsd(QString("geolocation"));

    preview->configScaling(20.0, 1000.0, 20.0, 50.0);
    preview->configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
}

void PreviewTerrainShape::updateEvent()
{
    TerrainRendererClass.bind(renderer, _terrain);
}

Color PreviewTerrainShape::getColor2D(double x, double y, double scaling)
{
    return terrainGetPreviewColor(renderer, x, y, scaling);
}
