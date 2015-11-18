#include "SoftwareCanvasRenderer.h"

#include "Rasterizer.h"
#include "SoftwareRenderer.h"
#include "Canvas.h"
#include "TerrainRasterizer.h"
#include "WaterRasterizer.h"
#include "SkyRasterizer.h"
#include "VegetationRasterizer.h"
#include "CameraDefinition.h"
#include "ParallelWork.h"
#include "CanvasPortion.h"
#include "CanvasPixelShader.h"
#include "RenderConfig.h"
#include "ColorProfile.h"
#include "CanvasPreview.h"
#include "RenderProgress.h"

SoftwareCanvasRenderer::SoftwareCanvasRenderer(Scenery *scenery) : SoftwareRenderer(scenery) {
    started = false;
    finished = false;
    interrupted = false;
    canvas = new Canvas();
    progress = new RenderProgress();
    samples = 1;

    postprocess_enabled = true;

    rasterizers.push_back(rasterizer_sky = new SkyRasterizer(this, progress, RASTERIZER_CLIENT_SKY));
    rasterizers.push_back(rasterizer_water = new WaterRasterizer(this, progress, RASTERIZER_CLIENT_WATER));
    rasterizers.push_back(rasterizer_terrain = new TerrainRasterizer(this, progress, RASTERIZER_CLIENT_TERRAIN));
    rasterizers.push_back(rasterizer_vegetation =
                              new VegetationRasterizer(this, progress, RASTERIZER_CLIENT_VEGETATION));

    current_work = NULL;
}

SoftwareCanvasRenderer::~SoftwareCanvasRenderer() {
    delete canvas;
    delete progress;

    delete rasterizer_sky;
    delete rasterizer_water;
    delete rasterizer_terrain;
    delete rasterizer_vegetation;
}

void SoftwareCanvasRenderer::setQuality(double factor) {
    SoftwareRenderer::setQuality(factor);

    for (auto &rasterizer : rasterizers) {
        rasterizer->setQuality(factor);
    }
}

void SoftwareCanvasRenderer::setSoloRasterizer(Rasterizer *rasterizer) {
    rasterizers.clear();
    rasterizers.push_back(rasterizer);
}

double SoftwareCanvasRenderer::getProgress() const {
    return progress->get();
}

void SoftwareCanvasRenderer::setConfig(const RenderConfig &config) {
    if (not started) {
        setSize(config.width, config.height, config.antialias);
        setQuality((double)(config.quality - 1) / 9.0);
    }
}

void SoftwareCanvasRenderer::enablePostprocess(bool enabled) {
    this->postprocess_enabled = enabled;
}

void SoftwareCanvasRenderer::setSize(int width, int height, int samples) {
    if (not started) {
        canvas->setSize(width * samples, height * samples);
        this->samples = samples;
    }
}

void SoftwareCanvasRenderer::render() {
    started = true;
    progress->reset();

    prepare();
    render_camera->setRenderSize(canvas->getWidth(), canvas->getHeight());

    // Iterate portions
    int nx = canvas->getHorizontalPortionCount();
    int ny = canvas->getVerticalPortionCount();
    int n = nx * ny;
    progress->enterSub(n);
    for (int y = 0; y < ny; y++) {
        for (int x = 0; x < nx; x++) {
            CanvasPortion *portion = canvas->at(x, y);

            progress->enterSub(2);

            if (not interrupted) {
                portion->preparePixels();
                rasterize(portion);
            }

            if (not interrupted and postprocess_enabled) {
                applyPixelShader(portion);
            }

            portion->discardPixels();

            progress->exitSub();
        }
    }
    progress->exitSub();
    progress->end();
    finished = true;
}

void SoftwareCanvasRenderer::interrupt() {
    interrupted = true;

    if (current_work) {
        current_work->interrupt();
    }
    for (auto &rasterizer : rasterizers) {
        rasterizer->interrupt();
    }
}

const Rasterizer &SoftwareCanvasRenderer::getRasterizer(int client_id) const {
    return *(rasterizers[client_id]);
}

bool SoftwareCanvasRenderer::saveToDisk(const std::string &filepath) const {
    return getCanvas()->saveToDisk(filepath, *getCanvas()->getPreview()->getToneMapping(), samples);
}

void SoftwareCanvasRenderer::rasterize(CanvasPortion *portion) {
    int count = 0;
    for (auto &rasterizer : rasterizers) {
        count += rasterizer->prepareRasterization();
    }

    progress->enterSub(count);
    for (auto &rasterizer : rasterizers) {
        rasterizer->rasterizeToCanvas(portion);
    }
    progress->exitSub();
}

void SoftwareCanvasRenderer::applyPixelShader(CanvasPortion *portion) {
    // Subdivide in chunks
    int chunk_size = 64;
    int chunks_x = (portion->getWidth() - 1) / chunk_size + 1;
    int chunks_y = (portion->getHeight() - 1) / chunk_size + 1;
    int units = chunks_x * chunks_y;

    // Render chunks in parallel
    progress->enterSub(portion->getWidth() * portion->getHeight());
    for (int sub_chunk_size = chunk_size; sub_chunk_size >= 1; sub_chunk_size /= 2) {
        if (interrupted) {
            break;
        }

        CanvasPixelShader shader(*this, portion, progress, chunk_size, sub_chunk_size, chunks_x, chunks_y);
        ParallelWork work(&shader, units);

        current_work = &work;
        work.perform();
        current_work = NULL;
    }
    progress->exitSub();
}
