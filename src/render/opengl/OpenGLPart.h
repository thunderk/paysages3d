#ifndef OPENGLPART_H
#define OPENGLPART_H

#include "opengl_global.h"

#include <map>
#include <vector>

namespace paysages {
namespace opengl {

/**
 * Class that can be inherited by scenery parts, to use OpenGL features.
 */
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

    // Free opengl resources generated in context (like textures...)
    virtual void destroy();

    // Interrupt the rendering
    virtual void interrupt();

    void updateScenery(bool onlyCommon = false);

  protected:
    /**
     * Create a shader program.
     *
     * The returned shader's ownership remains in this object. It will taks care of the destruction.
     */
    OpenGLShaderProgram *createShader(const string &name);

    /**
     * Create a vertex array.
     *
     * The returned array's ownership remains in this object. It will taks care of the destruction.
     */
    OpenGLVertexArray *createVertexArray(bool has_uv, bool strip);

    OpenGLFunctions *getFunctions();

    // Access to the main scenery renderer
    OpenGLRenderer *renderer;

  private:
    map<string, OpenGLShaderProgram *> shaders;
    vector<OpenGLVertexArray *> arrays;
};
}
}

#endif // OPENGLPART_H
