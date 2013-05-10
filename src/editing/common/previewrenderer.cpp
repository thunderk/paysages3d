#include "previewrenderer.h"

PreviewRenderer::PreviewRenderer(QObject *parent) :
    QObject(parent)
{
    renderer = rendererCreate();
}

PreviewRenderer::~PreviewRenderer()
{
    rendererDelete(renderer);
}

Color PreviewRenderer::getColor2D(double, double, double)
{
    return COLOR_BLACK;
}

void PreviewRenderer::bindEvent(BasePreview*)
{
}

void PreviewRenderer::updateEvent()
{
}
