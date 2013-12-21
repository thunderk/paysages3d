#ifndef OPENGLRENDERER_H
#define OPENGLRENDERER_H

#include "opengl_global.h"

#include "SoftwareRenderer.h"

class QOpenGLFunctions_3_2_Core;

namespace paysages {
namespace opengl {

/*!
 * \brief Scenery renderer in an OpenGL context.
 */
class OPENGLSHARED_EXPORT OpenGLRenderer: public SoftwareRenderer
{
public:
    OpenGLRenderer(Scenery* scenery=0);
    virtual ~OpenGLRenderer();

    void initialize();
    void resize(int width, int height);
    void paint();

    void cameraChangeEvent(CameraDefinition* camera);

    inline QOpenGLFunctions_3_2_Core* getOpenGlFunctions() {return functions;}

    virtual double getPrecision(const Vector3 &location) override;
    virtual Color applyMediumTraversal(Vector3 location, Color color) override;

private:
    QOpenGLFunctions_3_2_Core* functions;

    OpenGLSkybox* skybox;
};

}
}

#endif // OPENGLRENDERER_H
