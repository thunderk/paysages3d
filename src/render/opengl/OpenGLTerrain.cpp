#include "OpenGLTerrain.h"

#include "OpenGLFunctions.h"
#include "OpenGLRenderer.h"
#include "OpenGLShaderProgram.h"
#include "ParallelPool.h"
#include "Thread.h"
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

OpenGLTerrain::OpenGLTerrain(OpenGLRenderer *renderer) : OpenGLPart(renderer, "terrain") {
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

    for (int i = 0; i < _chunks.count(); i++) {
        delete _chunks[i];
    }
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
            _chunks.append(chunk);
            _updateQueue.append(chunk);
        }
    }

    // Start chunks maintenance
    work->start();

    // Watch for definition changes
    renderer->getScenery()->getTerrain()->propWaterHeight()->addWatcher(this);
    renderer->getScenery()->getAtmosphere()->propDayTime()->addWatcher(this);
}

void OpenGLTerrain::update() {
    for (auto &chunk : _chunks) {
        chunk->askReset(true, true);
    }
}

void OpenGLTerrain::render() {
    for (int i = 0; i < _chunks.count(); i++) {
        _chunks[i]->render(program);
    }
}

void OpenGLTerrain::interrupt() {
    for (auto &chunk : _chunks) {
        chunk->askInterrupt();
    }
}

void OpenGLTerrain::destroy() {
    OpenGLFunctions *functions = getFunctions();
    for (auto &chunk : _chunks) {
        chunk->destroy(functions);
    }
}

void OpenGLTerrain::pause() {
    paused = true;
    interrupt();
}

void OpenGLTerrain::resume() {
    for (auto &chunk : _chunks) {
        chunk->askResume();
    }
    paused = false;
}

void OpenGLTerrain::resetTextures() {
    for (auto &chunk : _chunks) {
        chunk->askReset(false, true);
    }
}

static bool _cmpChunks(const OpenGLTerrainChunk *c1, const OpenGLTerrainChunk *c2) {
    return c1->priority > c2->priority;
}

void OpenGLTerrain::performChunksMaintenance() {
    CameraDefinition *camera = renderer->getScenery()->getCamera();
    OpenGLTerrainChunk *chunk;

    _lock_chunks.lock();
    if (_updateQueue.count() > 0) {
        chunk = _updateQueue.takeFirst();
        _lock_chunks.unlock();
    } else {
        _lock_chunks.unlock();
        return;
    }

    chunk->maintain();

    _lock_chunks.lock();
    _updateQueue.append(chunk);
    for (int i = 0; i < _chunks.count(); i++) {
        _chunks[i]->updatePriority(camera);
    }
    qSort(_updateQueue.begin(), _updateQueue.end(), _cmpChunks);
    _lock_chunks.unlock();
}

void OpenGLTerrain::nodeChanged(const DefinitionNode *node, const DefinitionDiff *) {
    if (node->getPath() == "/terrain/water_height") {
        resetTextures();
    } else if (node->getPath() == "/atmosphere/daytime") {
        resetTextures();
    }
}
