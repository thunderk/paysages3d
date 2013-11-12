#include "previewrenderer.h"

#include "SoftwareRenderer.h"

PreviewRenderer::PreviewRenderer()
{
}

PreviewRenderer::~PreviewRenderer()
{
}

Color PreviewRenderer::getColor2D(double, double, double) const
{
    return COLOR_BLACK;
}

void PreviewRenderer::bindEvent(BasePreview*)
{
}

void PreviewRenderer::updateEvent()
{
    renderer->prepare();
}
