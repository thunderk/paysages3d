#include "OpenGLTerrain.h"

#include OPENGL_FUNCTIONS_INCLUDE
#include "OpenGLRenderer.h"
#include "OpenGLShaderProgram.h"
#include "ParallelPool.h"
#include "Thread.h"
#include "ExplorerChunkTerrain.h"
#include "WaterRenderer.h"
#include "CameraDefinition.h"
#include "Scenery.h"
#include "FloatNode.h"
#include "FloatDiff.h"

class ChunkMaintenanceThreads:public ParallelPool
{
public:
    ChunkMaintenanceThreads(OpenGLTerrain* terrain):
        terrain(terrain)
    {
    }

    virtual void work() override
    {
        while (running)
        {
            if (not terrain->isPaused())
            {
                terrain->performChunksMaintenance();
            }

            Thread::timeSleepMs(10);
        }
    }

private:
    OpenGLTerrain* terrain;
};

OpenGLTerrain::OpenGLTerrain(OpenGLRenderer *renderer):
    OpenGLPart(renderer)
{
    work = new ChunkMaintenanceThreads(this);
    paused = false;
}

OpenGLTerrain::~OpenGLTerrain()
{
    delete work;

    for (int i = 0; i < _chunks.count(); i++)
    {
        delete _chunks[i];
    }
}

void OpenGLTerrain::initialize()
{
    // Prepare shader programs
    program = createShader("terrain");
    program->addVertexSource("terrain");
    program->addFragmentSource("atmosphere");
    program->addFragmentSource("tonemapping");
    program->addFragmentSource("fadeout");
    program->addFragmentSource("terrain");

    // Add terrain chunks
    int chunks = 12;
    double size = 800.0;
    double chunksize = size / (double) chunks;
    double start = -size / 2.0;
    for (int i = 0; i < chunks; i++)
    {
        for (int j = 0; j < chunks; j++)
        {
            ExplorerChunkTerrain* chunk = new ExplorerChunkTerrain(renderer, start + chunksize * (double) i, start + chunksize * (double) j, chunksize, chunks);
            _chunks.append(chunk);
            _updateQueue.append(chunk);
        }
    }

    // Start chunks maintenance
    work->start();

    // Watch for definition changes
    renderer->getScenery()->getTerrain()->propWaterHeight()->addWatcher(this);
}

void OpenGLTerrain::update()
{
}

void OpenGLTerrain::render()
{
    program->bind();

    for (int i = 0; i < _chunks.count(); i++)
    {
        _chunks[i]->render(program->getProgram(), renderer->getOpenGlFunctions());
    }

    program->release();
}

void OpenGLTerrain::interrupt()
{
    for (auto &chunk: _chunks)
    {
        chunk->askInterrupt();
    }
}

void OpenGLTerrain::pause()
{
    paused = true;
    interrupt();
}

void OpenGLTerrain::resume()
{
    for (auto &chunk: _chunks)
    {
        chunk->askResume();
    }
    paused = false;
}

void OpenGLTerrain::resetTextures()
{
    for (auto &chunk: _chunks)
    {
        chunk->askReset(false, true);
    }
}

static bool _cmpChunks(const ExplorerChunkTerrain* c1, const ExplorerChunkTerrain* c2)
{
    return c1->priority > c2->priority;
}

void OpenGLTerrain::performChunksMaintenance()
{
    CameraDefinition* camera = renderer->getScenery()->getCamera();
    ExplorerChunkTerrain* chunk;

    _lock_chunks.lock();
    if (_updateQueue.count() > 0)
    {
        chunk = _updateQueue.takeFirst();
        _lock_chunks.unlock();
    }
    else
    {
        _lock_chunks.unlock();
        return;
    }

    chunk->maintain();

    _lock_chunks.lock();
    _updateQueue.append(chunk);
    for (int i = 0; i < _chunks.count(); i++)
    {
        _chunks[i]->updatePriority(camera);
    }
    qSort(_updateQueue.begin(), _updateQueue.end(), _cmpChunks);
    _lock_chunks.unlock();
}


void OpenGLTerrain::nodeChanged(const DefinitionNode *node, const DefinitionDiff *)
{
    if (node->getPath() == "/terrain/water_height")
    {
        resetTextures();
    }
}
