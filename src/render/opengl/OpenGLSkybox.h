#pragma once

#include "opengl_global.h"

#include "OpenGLPart.h"

namespace paysages {
namespace opengl {

class OPENGLSHARED_EXPORT OpenGLSkybox : public OpenGLPart {
  public:
    OpenGLSkybox(OpenGLRenderer *renderer);
    virtual ~OpenGLSkybox();

    virtual void initialize() override;
    virtual void update() override;
    virtual void render() override;

    virtual void nodeChanged(const DefinitionNode *node, const DefinitionDiff *diff,
                             const DefinitionNode *parent) override;
    virtual void floatNodeChanged(const string &path, double new_value, double old_value) override;

  private:
    OpenGLShaderProgram *program;
    OpenGLVertexArray *vertices;
};
}
}
