#include "PreviewLayerLook.h"

#include "SoftwareRenderer.h"
#include "BasePreview.h"

void PreviewLayerLook::setTextures(TexturesDefinition* textures)
{
    this->textures = textures;
}

void PreviewLayerLook::setLayer(int layer)
{
    this->layer = layer;
}

void PreviewLayerLook::bindEvent(BasePreview* preview)
{
    //preview->configHdrToneMapping(true);

    preview->configScaling(0.01, 1.0, 0.01, 0.1);
}

void PreviewLayerLook::updateEvent()
{
    if (textures)
    {
        //TexturesPreviewLayerLook_bind(this, textures);
    }
}

Color PreviewLayerLook::getColor2D(double x, double y, double scaling)
{
    if (textures)
    {
        return COLOR_BLACK;
        //return TexturesPreviewLayerLook_getColor(this, x, y, scaling, layer);
    }
    else
    {
        return COLOR_BLACK;
    }
}

