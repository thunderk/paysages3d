#ifndef OPENGLTERRAIN_H
#define OPENGLTERRAIN_H

#include "opengl_global.h"

#include "OpenGLPart.h"

#include <QVector>
#include <QList>
#include <QMutex>

namespace paysages {
namespace opengl {

class OPENGLSHARED_EXPORT OpenGLTerrain:public OpenGLPart
{
public:
    OpenGLTerrain(OpenGLRenderer* renderer);
    virtual ~OpenGLTerrain();

    virtual void initialize() override;
    virtual void update() override;
    virtual void render() override;
    virtual void interrupt() override;

    /**
     * Reset the color textures, without changing the tessellation.
     */
    void resetTextures();

    void performChunksMaintenance();

private:
    OpenGLShaderProgram* program;

    ParallelPool* work;

    QVector<ExplorerChunkTerrain*> _chunks;
    QList<ExplorerChunkTerrain*> _updateQueue;
    QMutex _lock_chunks;
};

}
}

#endif // OPENGLTERRAIN_H
