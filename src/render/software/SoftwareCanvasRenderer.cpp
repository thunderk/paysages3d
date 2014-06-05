#include "SoftwareCanvasRenderer.h"

#include "Rasterizer.h"
#include "SoftwareRenderer.h"
#include "Canvas.h"

SoftwareCanvasRenderer::SoftwareCanvasRenderer()
{
    started = false;
    renderer = new SoftwareRenderer();
    canvas = new Canvas();
}

SoftwareCanvasRenderer::~SoftwareCanvasRenderer()
{
    delete renderer;
    delete canvas;
}

void SoftwareCanvasRenderer::setSize(int width, int height, int samples)
{
    if (not started)
    {
        canvas->setSize(width * samples, height * samples);
    }
}

void SoftwareCanvasRenderer::render()
{
    // TEMP
    started = true;
    CanvasPortion *portion = canvas->at(0, 0);

    rasterize(portion, true);
    postProcess(portion, true);
}

void SoftwareCanvasRenderer::rasterize(CanvasPortion *portion, bool threaded)
{
    std::vector<Rasterizer> rasterizers;
    renderer->getRasterizers(&rasterizers);

    for (auto &rasterizer:rasterizers)
    {
    }
}

void SoftwareCanvasRenderer::postProcess(CanvasPortion *portion, bool threaded)
{
    // TODO
}
