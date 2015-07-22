#include "TextureLayerCoveragePreviewRenderer.h"

#include "Scenery.h"
#include "BasePreview.h"
#include "TexturesRenderer.h"

TextureLayerCoveragePreviewRenderer::TextureLayerCoveragePreviewRenderer():
    Base2dPreviewRenderer()
{
    textures = NULL;

    render_quality = 3;
}

void TextureLayerCoveragePreviewRenderer::setTextures(TexturesDefinition* textures)
{
    this->textures = textures;
}

void TextureLayerCoveragePreviewRenderer::setTerrain(TerrainDefinition *terrain)
{
    getScenery()->setTerrain(terrain);
}

void TextureLayerCoveragePreviewRenderer::setLayer(int layer)
{
    this->layer = layer;
}

void TextureLayerCoveragePreviewRenderer::bindEvent(BasePreview* preview)
{
    preview->addOsd(QString("geolocation"));

    preview->configScaling(20.0, 1000.0, 20.0, 200.0);
    preview->configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
}

void TextureLayerCoveragePreviewRenderer::updateEvent()
{
    Base2dPreviewRenderer::updateEvent();

    if (textures)
    {
        getScenery()->setTextures(textures);
    }

    disableAtmosphere();
    disableClouds();
}

Color TextureLayerCoveragePreviewRenderer::getColor2D(double x, double y, double)
{
    TexturesRenderer* textures_renderer = getTexturesRenderer();
    TerrainRenderer* terrain_renderer = getTerrainRenderer();

    double presence = textures_renderer->getBasePresence(layer, terrain_renderer->getResult(x, y, true, false));

    return Color(presence, presence, presence);
}
