#include "TexturesMixPreviewRenderer.h"

#include "Scenery.h"
#include "BasePreview.h"
#include "TerrainRenderer.h"

TexturesMixPreviewRenderer::TexturesMixPreviewRenderer():
    Base2dPreviewRenderer()
{
    textures = NULL;

    render_quality = 3;
}

void TexturesMixPreviewRenderer::setTextures(TexturesDefinition* textures)
{
    this->textures = textures;
}

void TexturesMixPreviewRenderer::setTerrain(TerrainDefinition *terrain)
{
    getScenery()->setTerrain(terrain);
}

void TexturesMixPreviewRenderer::setLayer(int layer)
{
    this->layer = layer;
}

void TexturesMixPreviewRenderer::bindEvent(BasePreview* preview)
{
    preview->addOsd(QString("geolocation"));

    preview->configHdrToneMapping(true);

    preview->configScaling(20.0, 1000.0, 20.0, 200.0);
    preview->configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
}

void TexturesMixPreviewRenderer::updateEvent()
{
    Base2dPreviewRenderer::updateEvent();

    if (textures)
    {
        getScenery()->setTextures(textures);
    }

    disableAtmosphere();
    disableClouds();
}

Color TexturesMixPreviewRenderer::getColor2D(double x, double y, double scaling)
{
    TerrainRenderer* terrain_renderer = getTerrainRenderer();
    Vector3 location(x, terrain_renderer->getHeight(x, y, 1), y);
    return terrain_renderer->getFinalColor(location, scaling);
}
