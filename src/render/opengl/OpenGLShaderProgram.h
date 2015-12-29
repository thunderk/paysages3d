#ifndef OPENGLSHADERPROGRAM_H
#define OPENGLSHADERPROGRAM_H

#include "opengl_global.h"

#include <string>

class QOpenGLShaderProgram;

namespace paysages {
namespace opengl {

class OPENGLSHARED_EXPORT OpenGLShaderProgram {
  public:
    OpenGLShaderProgram(const string &name, OpenGLRenderer *renderer);
    ~OpenGLShaderProgram();

    void addVertexSource(const string &path);
    void addFragmentSource(const string &path);

    /**
     * Release any allocated resource in the opengl context.
     *
     * Must be called in the opengl rendering thread, and before the destructor is called.
     */
    void destroy(OpenGLFunctions *functions);

    /**
     * Draw a VertexArray object.
     *
     * This will bind the program (compile it if needed), set the uniform variables, and
     * ask the array object to bind its VAO and render itself.
     *
     * *state* is optional and may add ponctual variables to the global state.
     */
    void draw(OpenGLVertexArray *vertices, OpenGLSharedState *state = NULL, int start = 0, int count = -1);

    /**
     * Check if the program is currently bound to OpenGL context.
     *
     * This does not really check the OpenGL context, just check we are between bind() and release() calls.
     */
    inline bool isBound() const {
        return bound;
    }

    /**
     * Get the OpenGL ID for this program.
     */
    unsigned int getId() const;

    inline QOpenGLShaderProgram *getProgram() const {
        return program;
    }
    inline OpenGLFunctions *getFunctions() const {
        return functions;
    }
    inline OpenGLRenderer *getRenderer() const {
        return renderer;
    }
    inline OpenGLSharedState *getState() const {
        return state;
    }

  protected:
    friend class OpenGLVariable;

  private:
    bool bind(OpenGLSharedState *state = NULL);
    void release();
    void compile();

    bool compiled;
    bool bound;

    OpenGLRenderer *renderer;

    string name;
    QOpenGLShaderProgram *program;
    OpenGLFunctions *functions;

    OpenGLSharedState *state;

    string source_vertex;
    string source_fragment;
};
}
}

#endif // OPENGLSHADERPROGRAM_H
