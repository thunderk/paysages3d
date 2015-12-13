#include "OpenGLTerrain.h"

#include <vector>
#include "OpenGLFunctions.h"
#include "OpenGLRenderer.h"
#include "OpenGLShaderProgram.h"
#include "ParallelPool.h"
#include "Thread.h"
#include "Mutex.h"
#include "OpenGLTerrainChunk.h"
#include "WaterRenderer.h"
#include "CameraDefinition.h"
#include "AtmosphereDefinition.h"
#include "Scenery.h"
#include "FloatNode.h"
#include "FloatDiff.h"

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
    double chunksize = size / (double)chunks;
    double start = -size / 2.0;
    for (int i = 0; i < chunks; i++) {
        for (int j = 0; j < chunks; j++) {
            OpenGLTerrainChunk *chunk = new OpenGLTerrainChunk(renderer, start + chunksize * (double)i,
                                                               start + chunksize * (double)j, chunksize, chunks);
            pv->chunks.push_back(chunk);
            pv->queue.push_back(chunk);
        }
    }

    // Start chunks maintenance
    work->start();

    // Watch for definition changes
    renderer->getScenery()->getTerrain()->propWaterHeight()->addWatcher(this);
    renderer->getScenery()->getAtmosphere()->propDayTime()->addWatcher(this);
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

void OpenGLTerrain::nodeChanged(const DefinitionNode *node, const DefinitionDiff *) {
    if (node->getPath() == "/terrain/water_height") {
        resetTextures();
    } else if (node->getPath() == "/atmosphere/daytime") {
        resetTextures();
    }
}
