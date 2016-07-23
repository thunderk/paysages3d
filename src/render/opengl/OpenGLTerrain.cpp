#include "OpenGLTerrain.h"

#include "Mutex.h"
#include "OpenGLFunctions.h"
#include "OpenGLRenderer.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLTerrainChunk.h"
#include "ParallelPool.h"
#include "Scenery.h"
#include "Thread.h"
#include <algorithm>
#include <vector>

class ChunkMaintenanceThreads : public ParallelPool {
  public:
    ChunkMaintenanceThreads(OpenGLTerrain *terrain) : terrain(terrain) {
    }

    virtual void work() override {
        while (running) {
            if (not terrain->isPaused()) {
                terrain->performChunksMaintenance();
            }

            Thread::timeSleepMs(10);
        }
    }

  private:
    OpenGLTerrain *terrain;
};

class OpenGLTerrainChunkCmp {
  public:
    bool operator()(const OpenGLTerrainChunk *lhs, const OpenGLTerrainChunk *rhs) const {
        return lhs->getPriority() < rhs->getPriority();
    }
};

class paysages::opengl::OpenGLTerrainPV {
  public:
    Mutex lock;
    vector<OpenGLTerrainChunk *> chunks;
    vector<OpenGLTerrainChunk *> queue;
};

OpenGLTerrain::OpenGLTerrain(OpenGLRenderer *renderer) : OpenGLPart(renderer, "terrain") {
    pv = new OpenGLTerrainPV();
    work = new ChunkMaintenanceThreads(this);
    paused = false;

    program = createShader("terrain");
    program->addVertexSource("terrain");
    program->addFragmentSource("atmosphere");
    program->addFragmentSource("tonemapping");
    program->addFragmentSource("fadeout");
    program->addFragmentSource("ui");
    program->addFragmentSource("terrain");
}

OpenGLTerrain::~OpenGLTerrain() {
    delete work;

    for (auto chunk : pv->chunks) {
        delete chunk;
    }

    delete pv;
}

void OpenGLTerrain::initialize() {
    // Add terrain chunks
    int chunks = 12;
    double size = 800.0;
    double chunksize = size / to_double(chunks);
    double start = -size / 2.0;
    for (int i = 0; i < chunks; i++) {
        for (int j = 0; j < chunks; j++) {
            OpenGLTerrainChunk *chunk = new OpenGLTerrainChunk(renderer, start + chunksize * to_double(i),
                                                               start + chunksize * to_double(j), chunksize, chunks);
            pv->chunks.push_back(chunk);
            pv->queue.push_back(chunk);
        }
    }

    // Start chunks maintenance
    work->start();

    // Watch for definition changes
    startWatching(renderer->getScenery(), "/terrain/water_height");
    startWatching(renderer->getScenery(), "/atmosphere/sun/phi");
    startWatching(renderer->getScenery(), "/atmosphere/sun/theta");
    startWatching(renderer->getScenery(), "/atmosphere/moon/phi");
    startWatching(renderer->getScenery(), "/atmosphere/moon/theta");
}

void OpenGLTerrain::update() {
    for (auto &chunk : pv->chunks) {
        chunk->askReset(true, true);
    }
}

void OpenGLTerrain::render() {
    for (auto chunk : pv->chunks) {
        chunk->render(program);
    }
}

void OpenGLTerrain::interrupt() {
    for (auto chunk : pv->chunks) {
        chunk->askInterrupt();
    }
}

void OpenGLTerrain::destroy() {
    OpenGLFunctions *functions = getFunctions();
    for (auto &chunk : pv->chunks) {
        chunk->destroy(functions);
    }
    OpenGLPart::destroy();
}

void OpenGLTerrain::pause() {
    paused = true;
    interrupt();
}

void OpenGLTerrain::resume() {
    for (auto &chunk : pv->chunks) {
        chunk->askResume();
    }
    paused = false;
}

void OpenGLTerrain::resetTextures() {
    for (auto &chunk : pv->chunks) {
        chunk->askReset(false, true);
    }
}

void OpenGLTerrain::performChunksMaintenance() {
    CameraDefinition *camera = renderer->getScenery()->getCamera();
    OpenGLTerrainChunk *chunk;

    pv->lock.acquire();
    for (auto &chunk : pv->chunks) {
        chunk->updatePriority(camera);
    }

    if (pv->queue.size() > 0) {
        sort(pv->queue.begin(), pv->queue.end(), OpenGLTerrainChunkCmp());

        chunk = pv->queue.back();
        pv->queue.pop_back();
    } else {
        chunk = NULL;
    }
    pv->lock.release();

    if (chunk) {
        chunk->maintain();

        pv->lock.acquire();
        pv->queue.push_back(chunk);
        pv->lock.release();
    }
}

void OpenGLTerrain::nodeChanged(const DefinitionNode *node, const DefinitionDiff *, const DefinitionNode *) {
    if (node->getPath() == "/terrain/water_height") {
        resetTextures();
    } else if (node->getPath().find("/atmosphere") == 0) {
        resetTextures();
    }
}
