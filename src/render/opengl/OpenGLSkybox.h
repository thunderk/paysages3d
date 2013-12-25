#ifndef OPENGLSKYBOX_H
#define OPENGLSKYBOX_H

#include "opengl_global.h"

#include "OpenGLPart.h"

namespace paysages {
namespace opengl {

class OPENGLSHARED_EXPORT OpenGLSkybox: public OpenGLPart
{
public:
    OpenGLSkybox(OpenGLRenderer* renderer);
    virtual ~OpenGLSkybox();

    virtual void initialize() override;
    virtual void update() override;
    virtual void render() override;

    void alterDayTime(double delta);

private:
    void setVertex(int i, float x, float y, float z);

    OpenGLShaderProgram* program;
    float* vertices;

    double daytime;
};

}
}

#endif // OPENGLSKYBOX_H
