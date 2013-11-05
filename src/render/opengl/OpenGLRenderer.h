#ifndef OPENGLRENDERER_H
#define OPENGLRENDERER_H

#include "opengl_global.h"

class Scenery;
class Renderer;

namespace paysages {
namespace opengl {

/*!
 * \brief Scenery renderer in an OpenGL context.
 */
class OPENGLSHARED_EXPORT OpenGLRenderer
{
public:
    OpenGLRenderer(Scenery* scenery=0);
    ~OpenGLRenderer();

    void initialize();
    void resize(int width, int height);
    void paint();

private:
    Scenery* scenery;
    Renderer* renderer;
};

}
}

#endif // OPENGLRENDERER_H
