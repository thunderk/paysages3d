#ifndef OPENGLRENDERER_H
#define OPENGLRENDERER_H

#include "opengl_global.h"

#include "SoftwareRenderer.h"

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

    inline OpenGLFunctions* getOpenGlFunctions() const {return functions;}
    inline OpenGLSharedState* getSharedState() const {return shared_state;}

    virtual double getPrecision(const Vector3 &location) override;
    virtual Color applyMediumTraversal(Vector3 location, Color color) override;

private:
    bool ready;

    OpenGLFunctions* functions;
    OpenGLSharedState* shared_state;

    OpenGLSkybox* skybox;
    OpenGLWater* water;
};

}
}

#endif // OPENGLRENDERER_H
