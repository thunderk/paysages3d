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
    preview->addOsd("geolocation");
    // TODO Translation
    preview->addToggle("highlight", "Coverage highlight", true);

    preview->configScaling(20.0, 1000.0, 20.0, 200.0);
    preview->configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
    // TODO Keep camera above center (for reflections)
}

void WaterCoveragePreviewRenderer::updateEvent()
{
    getScenery()->setWater(definition);

    RenderingScenery::getCurrent()->getTerrain(_terrain);
    TerrainShapePreviewRenderer::updateEvent();
}

Color WaterCoveragePreviewRenderer::getColor2D(double x, double y, double scaling)
{
    double height;

    height = terrain->getHeight(this, x, y, 1);
    if (height > terrain->getWaterHeight(this))
    {
        return TerrainShapePreviewRenderer::getColor2D(x, y, scaling);
    }
    else
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
}
