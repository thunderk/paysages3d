#ifndef OPENGLTERRAIN_H
#define OPENGLTERRAIN_H

#include "opengl_global.h"

#include "OpenGLPart.h"
#include "DefinitionWatcher.h"

namespace paysages {
namespace opengl {

class OpenGLTerrainPV;

class OPENGLSHARED_EXPORT OpenGLTerrain : public OpenGLPart, public DefinitionWatcher {
  public:
    OpenGLTerrain(OpenGLRenderer *renderer);
    virtual ~OpenGLTerrain();

    virtual void initialize() override;
    virtual void update() override;
    virtual void render() override;
    virtual void interrupt() override;
    virtual void destroy() override;

    virtual void pause() override;
    virtual void resume() override;
    inline bool isPaused() const {
        return paused;
    }

    /**
     * Reset the color textures, without changing the tessellation.
     */
    void resetTextures();

    void performChunksMaintenance();

    virtual void nodeChanged(const DefinitionNode *node, const DefinitionDiff *diff, const DefinitionNode *) override;

  private:
    OpenGLShaderProgram *program;

    ParallelPool *work;
    bool paused;

    OpenGLTerrainPV *pv;
};
}
}

#endif // OPENGLTERRAIN_H
