#ifndef OPENGLRENDERER_H
#define OPENGLRENDERER_H

#include "opengl_global.h"

#include "SoftwareRenderer.h"

namespace paysages {
namespace opengl {

/*!
 * \brief Scenery renderer in an OpenGL context.
 */
class OPENGLSHARED_EXPORT OpenGLRenderer: protected SoftwareRenderer
{
public:
    OpenGLRenderer(Scenery* scenery=0);
    virtual ~OpenGLRenderer();

    void initialize();
    void resize(int width, int height);
    void paint();
};

}
}

#endif // OPENGLRENDERER_H
