#include "PreviewLayerCoverage.h"

#include "SoftwareRenderer.h"
#include "basepreview.h"
#include "textures/tex_preview.h"

void PreviewLayerCoverage::setTextures(TexturesDefinition* textures)
{
    this->textures = textures;
}

void PreviewLayerCoverage::setLayer(int layer)
{
    this->layer = layer;
}

void PreviewLayerCoverage::bindEvent(BasePreview* preview)
{
    preview->addOsd(QString("geolocation"));

    preview->configScaling(20.0, 1000.0, 20.0, 200.0);
    preview->configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
}

void PreviewLayerCoverage::updateEvent()
{
    if (textures)
    {
        TexturesPreviewLayerCoverage_bind(renderer, textures);
    }
}

Color PreviewLayerCoverage::getColor2D(double x, double y, double scaling)
{
    if (textures)
    {
        return TexturesPreviewLayerCoverage_getColor(renderer, x, y, scaling, layer);
    }
    else
    {
        return COLOR_BLACK;
    }
}
