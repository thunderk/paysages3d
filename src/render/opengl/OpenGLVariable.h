#ifndef OPENGLVARIABLE_H
#define OPENGLVARIABLE_H

#include "opengl_global.h"

#include <QColor>
#include <QVector3D>
#include <QMatrix4x4>

namespace paysages {
namespace opengl {

/*!
 * \brief OpenGL variable that can be bound to a uniform for shaders.
 */
class OpenGLVariable
{
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

    void apply(OpenGLShaderProgram *program);

    void set(const Texture2D *texture);
    void set(const Texture3D *texture);
    void set(const Texture4D *texture);
    void set(float value);
    void set(const Vector3 &vector);
    void set(const QVector3D &vector);
    void set(const Matrix4 &matrix);
    void set(const QMatrix4x4 &matrix);
    void set(const Color &color);

private:
    std::string name;
    OpenGLVariableType type;

    float value_float;
    QColor value_color;
    QVector3D value_vector3;
    QMatrix4x4 value_matrix4;
};

}
}

#endif // OPENGLVARIABLE_H
