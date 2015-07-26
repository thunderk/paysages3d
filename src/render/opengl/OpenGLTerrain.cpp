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
            terrain->performChunksMaintenance();
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
    int chunks = 20;
    double size = 800.0;
    double chunksize = size / (double) chunks;
    double start = -size / 2.0;
    double water_height = renderer->getWaterRenderer()->getHeightInfo().base_height;
    for (int i = 0; i < chunks; i++)
    {
        for (int j = 0; j < chunks; j++)
        {
            ExplorerChunkTerrain* chunk = new ExplorerChunkTerrain(renderer, start + chunksize * (double) i, start + chunksize * (double) j, chunksize, chunks, water_height);
            _chunks.append(chunk);
            _updateQueue.append(chunk);
        }
    }

    // Start chunks maintenance
    work->start();
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
