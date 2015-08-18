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
#include "ColorProfile.h"
#include "CanvasPreview.h"

SoftwareCanvasRenderer::SoftwareCanvasRenderer(Scenery *scenery):
    SoftwareRenderer(scenery)
{
    started = false;
    finished = false;
    interrupted = false;
    canvas = new Canvas();
    progress = 0.0;
    samples = 1;

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
        this->samples = samples;
    }
}

void SoftwareCanvasRenderer::render()
{
    started = true;
    progress = 0.0;

    render_camera->setRenderSize(canvas->getWidth(), canvas->getHeight());

    prepare();

    // Iterate portions
    int nx = canvas->getHorizontalPortionCount();
    int ny = canvas->getVerticalPortionCount();
    int i = 0;
    int n = nx * ny;
    for (int y = 0; y < ny; y++)
    {
        for (int x = 0; x < nx; x++)
        {
            CanvasPortion *portion = canvas->at(x, y);

            if (not interrupted)
            {
                progress_segment = 0.2 / (double)n;
                portion->preparePixels();
                rasterize(portion);
            }

            if (not interrupted)
            {
                progress_segment = 0.8 / (double)n;
                applyPixelShader(portion);
            }

            portion->discardPixels();
            i++;
            progress = (double)i / (double)n;
        }
    }
    finished = true;
}

void SoftwareCanvasRenderer::interrupt()
{
    interrupted = true;

    if (current_work)
    {
        current_work->interrupt();
    }
    for (auto &rasterizer:rasterizers)
    {
        rasterizer->interrupt();
    }
}

const Rasterizer &SoftwareCanvasRenderer::getRasterizer(int client_id) const
{
    return *(rasterizers[client_id]);
}

bool SoftwareCanvasRenderer::saveToDisk(const std::string &filepath) const
{
    return getCanvas()->saveToDisk(filepath, *getCanvas()->getPreview()->getToneMapping(), samples);
}

void SoftwareCanvasRenderer::rasterize(CanvasPortion *portion)
{
    for (auto &rasterizer:rasterizers)
    {
        rasterizer->rasterizeToCanvas(portion);
        progress += progress_segment / (double)rasterizers.size();
    }
}

void SoftwareCanvasRenderer::applyPixelShader(CanvasPortion *portion)
{
    // Subdivide in chunks
    int chunk_size = 64;
    int chunks_x = (portion->getWidth() - 1) / chunk_size + 1;
    int chunks_y = (portion->getHeight() - 1) / chunk_size + 1;
    int units = chunks_x * chunks_y;

    // Estimate chunks
    int n = 0;
    for (int sub_chunk_size = chunk_size; sub_chunk_size >= 1; sub_chunk_size /= 2)
    {
        n += chunk_size / sub_chunk_size;
    }

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
        progress += progress_segment * (double)(chunk_size / sub_chunk_size) / (double)n;
    }
}
