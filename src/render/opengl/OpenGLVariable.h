#ifndef OPENGLVARIABLE_H
#define OPENGLVARIABLE_H

#include "opengl_global.h"

class QColor;
class QVector3D;
class QMatrix4x4;
class QImage;

namespace paysages {
namespace opengl {

/*!
 * \brief OpenGL variable that can be bound to a uniform for shaders.
 */
class OpenGLVariable {
  public:
    typedef enum {
        TYPE_NONE,
        TYPE_TEXTURE_2D,
        TYPE_TEXTURE_3D,
        TYPE_TEXTURE_4D,
        TYPE_FLOAT,
        TYPE_VECTOR3,
        TYPE_MATRIX4,
        TYPE_COLOR
    } OpenGLVariableType;

  public:
    OpenGLVariable(const std::string &name);
    ~OpenGLVariable();

    void apply(OpenGLShaderProgram *program, int &texture_unit);

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
    void set(float value);
    void set(const Vector3 &vector);
    void set(const QVector3D &vector);
    void set(const Matrix4 &matrix);
    void set(const QMatrix4x4 &matrix);
    void set(const Color &color);

  protected:
    void uploadTexture(OpenGLRenderer *renderer);

  private:
    std::string name;
    OpenGLVariableType type;

    float value_float;
    QColor *value_color;
    QVector3D *value_vector3;
    QMatrix4x4 *value_matrix4;
    float *value_texture_data;

    int texture_size_x;
    int texture_size_y;
    int texture_size_z;
    bool texture_toupload;
    bool texture_repeat;
    bool texture_color;
    unsigned int texture_id;
};
}
}

#endif // OPENGLVARIABLE_H
