#include "WaterCoveragePreviewRenderer.h"

#include "BasePreview.h"
#include "Scenery.h"
#include "TerrainDefinition.h"
#include "WaterRenderer.h"

WaterCoveragePreviewRenderer::WaterCoveragePreviewRenderer(WaterDefinition* definition):
    TerrainShapePreviewRenderer(new TerrainDefinition(NULL)), definition(definition)
{
    highlight = true;
}

WaterCoveragePreviewRenderer::~WaterCoveragePreviewRenderer()
{
    delete TerrainShapePreviewRenderer::_terrain;
}

void WaterCoveragePreviewRenderer::setTerrain(TerrainDefinition *terrain)
{
    terrain->copy(TerrainShapePreviewRenderer::_terrain);
    getScenery()->setTerrain(terrain);
}

void WaterCoveragePreviewRenderer::bindEvent(BasePreview* preview)
{
    TerrainShapePreviewRenderer::bindEvent(preview);

    // TODO Translation
    preview->addToggle("highlight", "Coverage highlight", true);

    // TODO Keep camera above center (for reflections)
}

void WaterCoveragePreviewRenderer::toggleChangeEvent(const std::string &key, bool value)
{
    if (key == "highlight")
    {
        highlight = value;
    }
}

void WaterCoveragePreviewRenderer::updateEvent()
{
    getScenery()->setWater(definition);

    TerrainShapePreviewRenderer::updateEvent();
}

Color WaterCoveragePreviewRenderer::getWaterColor(double x, double y, double)
{
    Color base;

    base = getWaterRenderer()->getResult(x, y).final;

    if (highlight)
    {
        Color mask = {0.5, 0.5, 1.0, 0.5};
        base.mask(mask);
    }

    return base;
}
