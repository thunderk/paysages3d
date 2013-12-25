#ifndef OPENGLWATER_H
#define OPENGLWATER_H

#include "opengl_global.h"

#include "OpenGLPart.h"

namespace paysages {
namespace opengl {

class OPENGLSHARED_EXPORT OpenGLWater: public OpenGLPart
{
public:
    OpenGLWater(OpenGLRenderer* renderer);
    virtual ~OpenGLWater();

    virtual void initialize() override;
    virtual void update() override;
    virtual void render() override;

private:
    void setVertex(int i, float x, float y, float z);

    OpenGLShaderProgram* program;
    float* vertices;
};

}
}

#endif // OPENGLWATER_H
