#ifndef OPENGLPART_H
#define OPENGLPART_H

#include "opengl_global.h"

#include <QMap>
#include <QString>

namespace paysages {
namespace opengl {

// Class that can be inherited by scenery parts, to use OpenGL features

class OPENGLSHARED_EXPORT OpenGLPart {
  public:
    OpenGLPart(OpenGLRenderer *renderer);
    virtual ~OpenGLPart();

    // Initialize the part rendering (create shaders, prepare static textures...)
    virtual void initialize() = 0;

    // Update parameters from scenery
    virtual void update() = 0;

    // Do the rendering
    virtual void render() = 0;

    // Interrupt the rendering
    virtual void interrupt();

    void updateScenery(bool onlyCommon = false);

    /**
     * Get access to rendered scenery.
     */
    Scenery *getScenery() const;

    /**
     * Get access to OpenGL functions.
     */
    OpenGLFunctions *getOpenGlFunctions() const;

  protected:
    // Create a shader program
    OpenGLShaderProgram *createShader(QString name);

    // Access to the main scenery renderer
    OpenGLRenderer *renderer;

  private:
    QMap<QString, OpenGLShaderProgram *> shaders;
};
}
}

#endif // OPENGLPART_H
