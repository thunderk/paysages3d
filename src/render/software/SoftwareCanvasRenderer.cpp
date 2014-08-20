#include "SoftwareCanvasRenderer.h"

#include "Rasterizer.h"
#include "SoftwareRenderer.h"
#include "Canvas.h"
#include "TerrainRasterizer.h"
#include "WaterRasterizer.h"
#include "SkyRasterizer.h"
#include "CameraDefinition.h"
#include "ParallelWork.h"
#include "CanvasPortion.h"
#include "CanvasPixelShader.h"
#include "RenderConfig.h"

SoftwareCanvasRenderer::SoftwareCanvasRenderer()
{
    started = false;
    interrupted = false;
    canvas = new Canvas();

    rasterizers.push_back(new SkyRasterizer(this, 0));
    rasterizers.push_back(new WaterRasterizer(this, 1));
    rasterizers.push_back(new TerrainRasterizer(this, 2));

    current_work = NULL;
}

SoftwareCanvasRenderer::~SoftwareCanvasRenderer()
{
    delete canvas;

    for (auto &rasterizer: rasterizers)
    {
        delete rasterizer;
    }
}

void SoftwareCanvasRenderer::setConfig(const RenderConfig &config)
{
    if (not started)
    {
        setSize(config.width, config.height, config.antialias);
        render_quality = config.quality;
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
    started = true;

    render_camera->setRenderSize(canvas->getWidth(), canvas->getHeight());

    prepare();

    // Iterate portions
    int nx = canvas->getHorizontalPortionCount();
    int ny = canvas->getVerticalPortionCount();
    for (int y = 0; y < ny; y++)
    {
        for (int x = 0; x < nx; x++)
        {
            CanvasPortion *portion = canvas->at(x, y);

            if (not interrupted)
            {
                portion->preparePixels();
                rasterize(portion, true);
            }

            if (not interrupted)
            {
                applyPixelShader(portion, true);
            }

            portion->discardPixels();
        }
    }
}

void SoftwareCanvasRenderer::interrupt()
{
    interrupted = true;

    if (current_work)
    {
        current_work->interrupt();
    }
    for (auto &rasterizer:getRasterizers())
    {
        rasterizer->interrupt();
    }
}

const std::vector<Rasterizer *> &SoftwareCanvasRenderer::getRasterizers() const
{
    return rasterizers;
}

const Rasterizer &SoftwareCanvasRenderer::getRasterizer(int client_id) const
{
    return *(getRasterizers()[client_id]);
}

void SoftwareCanvasRenderer::rasterize(CanvasPortion *portion, bool threaded)
{
    for (auto &rasterizer:getRasterizers())
    {
        rasterizer->rasterizeToCanvas(portion);
    }
}

void SoftwareCanvasRenderer::applyPixelShader(CanvasPortion *portion, bool threaded)
{
    // Subdivide in chunks
    int chunk_size = 64;
    int chunks_x = (portion->getWidth() - 1) / chunk_size + 1;
    int chunks_y = (portion->getHeight() - 1) / chunk_size + 1;
    int units = chunks_x * chunks_y;

    // Render chunks in parallel
    for (int sub_chunk_size = chunk_size; sub_chunk_size >= 1; sub_chunk_size /= 2)
    {
        if (interrupted)
        {
            break;
        }

        CanvasPixelShader shader(*this, portion, chunk_size, sub_chunk_size, chunks_x, chunks_y);
        ParallelWork work(&shader, units);

        current_work = &work;
        work.perform();
        current_work = NULL;
    }
}
