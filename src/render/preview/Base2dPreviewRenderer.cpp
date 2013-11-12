#include "Base2dPreviewRenderer.h"

#include "SoftwareRenderer.h"

Base2dPreviewRenderer::Base2dPreviewRenderer()
{
    renderer = new SoftwareRenderer();
}

void Base2dPreviewRenderer::updateEvent()
{
    delete renderer;
}

Color Base2dPreviewRenderer::getColor2D(double, double, double) const
{
    return COLOR_BLACK;
}
