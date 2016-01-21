#ifndef OPENGLWATER_H
#define OPENGLWATER_H

#include "opengl_global.h"

#include "OpenGLPart.h"

namespace paysages {
namespace opengl {

class OPENGLSHARED_EXPORT OpenGLWater : public OpenGLPart {
  public:
    OpenGLWater(OpenGLRenderer *renderer);
    virtual ~OpenGLWater();

    virtual void initialize() override;
    virtual void update() override;
    virtual void render() override;

    virtual void nodeChanged(const DefinitionNode *node, const DefinitionDiff *diff,
                             const DefinitionNode *parent) override;

    /**
     * Enable or disable the water surface rendering.
     */
    void setEnabled(bool enabled);

  private:
    bool enabled;
    OpenGLShaderProgram *program;
    OpenGLVertexArray *vertices;
};
}
}

#endif // OPENGLWATER_H
