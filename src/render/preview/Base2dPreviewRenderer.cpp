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

void Base2dPreviewRenderer::toggleChangeEvent(const std::string &, bool)
{
}

void Base2dPreviewRenderer::choiceChangeEvent(const std::string &, int)
{
}

void Base2dPreviewRenderer::updateEvent()
{
}

void Base2dPreviewRenderer::cameraEvent(double, double, double)
{
}

Color Base2dPreviewRenderer::getColor2D(double, double, double)
{
    return COLOR_BLACK;
}
