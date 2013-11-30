#include "WaterCoveragePreviewRenderer.h"

#include "BasePreview.h"
#include "Scenery.h"
#include "TerrainDefinition.h"

// TEMP
#include "RenderingScenery.h"
#include "terrain/public.h"
#include "water/public.h"

WaterCoveragePreviewRenderer::WaterCoveragePreviewRenderer(WaterDefinition* definition):
    TerrainShapePreviewRenderer(new TerrainDefinition(NULL)), definition(definition)
{
    highlight = true;
}

WaterCoveragePreviewRenderer::~WaterCoveragePreviewRenderer()
{
    delete TerrainShapePreviewRenderer::_terrain;
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
    RenderingScenery::getCurrent()->getTerrain(_terrain);
    TerrainShapePreviewRenderer::updateEvent();

    getScenery()->setWater(definition);
    WaterRendererClass.bind(this, definition);
}

Color WaterCoveragePreviewRenderer::getWaterColor(double x, double y, double)
{
    Color base;

    base = water->getResult(this, x, y).final;

    if (highlight)
    {
        Color mask = {0.5, 0.5, 1.0, 0.5};
        colorMask(&base, &mask);
    }

    return base;
}
