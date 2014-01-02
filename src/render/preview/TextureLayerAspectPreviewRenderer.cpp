#include "TextureLayerAspectPreviewRenderer.h"

#include "Scenery.h"
#include "BasePreview.h"
#include "TexturesRenderer.h"

TextureLayerAspectPreviewRenderer::TextureLayerAspectPreviewRenderer():
    Base2dPreviewRenderer()
{
    textures = NULL;

    render_quality = 3;
}

void TextureLayerAspectPreviewRenderer::setTextures(TexturesDefinition* textures)
{
    this->textures = textures;
}

void TextureLayerAspectPreviewRenderer::setLayer(int layer)
{
    this->layer = layer;
}

void TextureLayerAspectPreviewRenderer::bindEvent(BasePreview* preview)
{
    preview->addOsd(QString("geolocation"));

    preview->configScaling(20.0, 1000.0, 20.0, 200.0);
    preview->configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
}

void TextureLayerAspectPreviewRenderer::updateEvent()
{
    Base2dPreviewRenderer::updateEvent();

    if (textures)
    {
        getScenery()->setTextures(textures);
    }

    disableAtmosphere();
    disableClouds();
}

Color TextureLayerAspectPreviewRenderer::getColor2D(double x, double y, double)
{
    TexturesRenderer* textures_renderer = getTexturesRenderer();

    return textures_renderer->applyToTerrain(x, y).final_color;
}
