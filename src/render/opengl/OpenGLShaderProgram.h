#ifndef OPENGLSHADERPROGRAM_H
#define OPENGLSHADERPROGRAM_H

#include "opengl_global.h"

class QOpenGLShaderProgram;

namespace paysages {
namespace opengl {

class OPENGLSHARED_EXPORT OpenGLShaderProgram {
  public:
    OpenGLShaderProgram(const std::string &name, OpenGLRenderer *renderer);
    ~OpenGLShaderProgram();

    void addVertexSource(const std::string &path);
    void addFragmentSource(const std::string &path);

    /**
     * Draw a VertexArray object.
     *
     * This will bind the program (compile it if needed), set the uniform variables, and
     * ask the array object to bind its VAO and render itself.
     *
     * *state* is optional and may add ponctual variables to the global state.
     */
    void draw(OpenGLVertexArray *vertices, OpenGLSharedState *state = NULL, int start=0, int count=-1);

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

    OpenGLRenderer *renderer;

    std::string name;
    QOpenGLShaderProgram *program;
    OpenGLFunctions *functions;

    OpenGLSharedState *state;

    std::string source_vertex;
    std::string source_fragment;
};
}
}

#endif // OPENGLSHADERPROGRAM_H
