#ifndef OPENGLSKYBOX_H
#define OPENGLSKYBOX_H

#include "opengl_global.h"

#include "OpenGLPart.h"
#include "DefinitionWatcher.h"

namespace paysages {
namespace opengl {

class OPENGLSHARED_EXPORT OpenGLSkybox : public OpenGLPart, public DefinitionWatcher {
  public:
    OpenGLSkybox(OpenGLRenderer *renderer);
    virtual ~OpenGLSkybox();

    virtual void initialize() override;
    virtual void update() override;
    virtual void render() override;

    virtual void nodeChanged(const DefinitionNode *node, const DefinitionDiff *diff) override;

  private:
    void setVertex(int i, float x, float y, float z);

    OpenGLShaderProgram *program;
    float *vertices;
};
}
}

#endif // OPENGLSKYBOX_H
