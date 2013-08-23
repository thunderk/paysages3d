#include "PreviewCumul.h"

#include "editing/basepreview.h"
#include "rendering/textures/tex_preview.h"

void PreviewCumul::setTextures(TexturesDefinition* textures)
{
    this->textures = textures;
}

void PreviewCumul::setLayer(int layer)
{
    this->layer = layer;
}

void PreviewCumul::bindEvent(BasePreview* preview)
{
    preview->addOsd(QString("geolocation"));

    //preview->configHdrToneMapping(true);

    preview->configScaling(20.0, 1000.0, 20.0, 200.0);
    preview->configScrolling(-1000.0, 1000.0, 0.0, -1000.0, 1000.0, 0.0);
}

void PreviewCumul::updateEvent()
{
    if (textures)
    {
        TexturesPreviewCumul_bind(renderer, textures);
    }
}

Color PreviewCumul::getColor2D(double x, double y, double scaling)
{
    if (textures)
    {
        return TexturesPreviewCumul_getColor(renderer, x, y, scaling, layer);
    }
    else
    {
        return COLOR_BLACK;
    }
}
