#include "SoftwareCanvasRenderer.h"

#include "Rasterizer.h"
#include "SoftwareRenderer.h"
#include "Canvas.h"
#include "TerrainRasterizer.h"
#include "WaterRasterizer.h"
#include "SkyRasterizer.h"
#include "CameraDefinition.h"

SoftwareCanvasRenderer::SoftwareCanvasRenderer()
{
    started = false;
    canvas = new Canvas();

    rasterizers.push_back(new TerrainRasterizer(this, 0));
    rasterizers.push_back(new WaterRasterizer(this, 1));
    rasterizers.push_back(new SkyRasterizer(this, 2));
}

SoftwareCanvasRenderer::~SoftwareCanvasRenderer()
{
    delete canvas;

    for (auto &rasterizer: rasterizers)
    {
        delete rasterizer;
    }
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

    render_camera->setRenderSize(canvas->getWidth(), canvas->getHeight());

    prepare();

    rasterize(portion, true);
    postProcess(portion, true);
}

const std::vector<Rasterizer *> &SoftwareCanvasRenderer::getRasterizers() const
{
    return rasterizers;
}

void SoftwareCanvasRenderer::rasterize(CanvasPortion *portion, bool threaded)
{
    for (auto &rasterizer:getRasterizers())
    {
        rasterizer->rasterizeToCanvas(portion);
    }
}

void SoftwareCanvasRenderer::postProcess(CanvasPortion *portion, bool threaded)
{
    // TODO
}
