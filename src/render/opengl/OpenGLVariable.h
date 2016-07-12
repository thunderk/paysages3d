#pragma once

#include "opengl_global.h"

#include <memory>

class QImage;

namespace paysages {
namespace opengl {

/**
 * OpenGL variable that can be bound to a uniform for shaders.
 */
class OPENGLSHARED_EXPORT OpenGLVariable final {
  public:
    OpenGLVariable(const string &name);
    ~OpenGLVariable();

    /**
     * Apply the variable to the bound shader program.
     *
     * 'texture_unit' will be used and updated accordingly.
     *
     * This must be called from the rendering thread, with the shader program bound.
     */
    void apply(OpenGLShaderProgram *program, unsigned int &texture_unit);

    /**
     * Release any allocated resource in the opengl context.
     *
     * Must be called in the opengl rendering thread, and before the destructor is called.
     */
    void destroy(OpenGLFunctions *functions);

    void set(const Texture2D *texture, bool repeat = false, bool color = true);
    void set(const QImage &texture, bool repeat = false, bool color = true);
    void set(const Texture3D *texture, bool repeat = false, bool color = true);
    void set(const Texture4D *texture, bool repeat = false, bool color = true);
    void set(int value);
    void set(float value);
    void set(const FractalNoise &noise);
    void set(const Vector3 &vector);
    void set(const Matrix4 &matrix);
    void set(const Color &color);

    int getIntValue() const;
    float getFloatValue() const;
    float getFloatArrayValue(unsigned int index) const;

  protected:
    void uploadTexture(OpenGLFunctions *renderer);

  private:
    class pimpl;
    unique_ptr<pimpl> impl;
};
}
}
