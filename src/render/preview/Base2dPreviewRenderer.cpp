#include "Base2dPreviewRenderer.h"

#include "Scenery.h"
#include "SoftwareRenderer.h"

Base2dPreviewRenderer::Base2dPreviewRenderer():
    SoftwareRenderer()
{
}

Base2dPreviewRenderer::~Base2dPreviewRenderer()
{
}

void Base2dPreviewRenderer::bindEvent(BasePreview*)
{
}

void Base2dPreviewRenderer::updateEvent()
{
}

Color Base2dPreviewRenderer::getColor2D(double, double, double)
{
    return COLOR_BLACK;
}
