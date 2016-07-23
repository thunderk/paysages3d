#pragma once

#include "opengl_global.h"

#include "OpenGLVariable.h"
#include <map>

namespace paysages {
namespace opengl {

/**
 * OpenGL variables that can be shared between shaders.
 */
class OPENGLSHARED_EXPORT OpenGLSharedState {
  public:
    OpenGLSharedState();
    ~OpenGLSharedState();

    /**
     * Apply the stored variables to the bound program.
     */
    void apply(OpenGLShaderProgram *program, unsigned int &texture_unit);

    /**
     * Release any allocated resource in the opengl context.
     *
     * Must be called in the opengl rendering thread, and before the destructor is called.
     */
    void destroy(OpenGLFunctions *functions);

    /**
     * Get or create a variable in the state.
     */
    OpenGLVariable *get(const string &name);

    // Shortcuts
    inline void setInt(const string &name, int value) {
        get(name)->set(value);
    }
    inline void set(const string &name, float value) {
        get(name)->set(value);
    }
    inline void set(const string &name, const Texture2D *texture, bool repeat = false, bool color = true) {
        get(name)->set(texture, repeat, color);
    }
    inline void set(const string &name, const QImage &texture, bool repeat = false, bool color = true) {
        get(name)->set(texture, repeat, color);
    }
    inline void set(const string &name, const Texture3D *texture, bool repeat = false, bool color = true) {
        get(name)->set(texture, repeat, color);
    }
    inline void set(const string &name, const Texture4D *texture, bool repeat = false, bool color = true) {
        get(name)->set(texture, repeat, color);
    }
    inline void set(const string &name, const Vector3 &vector) {
        get(name)->set(vector);
    }
    inline void set(const string &name, const Matrix4 &matrix) {
        get(name)->set(matrix);
    }
    inline void set(const string &name, const Color &color) {
        get(name)->set(color);
    }
    inline void set(const string &name, const FractalNoise &noise) {
        get(name)->set(noise);
    }

  private:
    map<string, OpenGLVariable *> variables;
};
}
}
