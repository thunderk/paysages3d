#include "previewterrainshape.h"

#include "basepreview.h"
#include "rendering/water/public.h"

PreviewTerrainShape::PreviewTerrainShape(TerrainDefinition* terrain)
{
    _terrain = terrain;
    _highlight_enabled = true;

    // TODO Don't delete the base renderer, just alter it
    rendererDelete(renderer);
    renderer = terrainCreatePreviewRenderer();
}

void PreviewTerrainShape::bindEvent(BasePreview* preview)
{
    preview->addOsd(QString("geolocation"));
    //preview->addToggle("highlight", tr("Coverage highlight"), true);

    preview->configScaling(20.0, 1000.0, 20.0, 200.0);
    preview->configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
}

void PreviewTerrainShape::updateEvent()
{
    TerrainRendererClass.bind(renderer, _terrain);
}

Color PreviewTerrainShape::getColor2D(double x, double y, double scaling)
{
    return waterGetPreviewCoverage(renderer, x, y, scaling, _highlight_enabled ? 1 : 0);
}

void PreviewTerrainShape::toggleChangeEvent(QString key, bool value)
{
    if (key == "highlight")
    {
        _highlight_enabled = value;
    }
}
