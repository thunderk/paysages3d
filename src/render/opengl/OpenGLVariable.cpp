#include "OpenGLVariable.h"

#include <cassert>
#include <QOpenGLShaderProgram>
#include "OpenGLShaderProgram.h"
#include "Vector3.h"
#include "Color.h"

OpenGLVariable::OpenGLVariable(const std::string &name):
    name(name)
{
    type = TYPE_NONE;
}

void OpenGLVariable::apply(OpenGLShaderProgram *program)
{
    QOpenGLShaderProgram* pr = program->getProgram();

    switch (type)
    {
    case TYPE_FLOAT:
        pr->setUniformValue(name.c_str(), value_float);
        break;
    case TYPE_COLOR:
        pr->setUniformValue(name.c_str(), value_color);
        break;
    case TYPE_VECTOR3:
        pr->setUniformValue(name.c_str(), value_vector3);
        break;
    case TYPE_NONE:
        break;
    }
}

void OpenGLVariable::set(float value)
{
    assert(type == TYPE_NONE or type == TYPE_FLOAT);

    type = TYPE_FLOAT;
    value_float = value;
}

void OpenGLVariable::set(const Vector3 &vector)
{
    assert(type == TYPE_NONE or type == TYPE_COLOR);

    type = TYPE_VECTOR3;
    value_vector3 = QVector3D(vector.x, vector.y, vector.z);
}

void OpenGLVariable::set(const Color &color)
{
    assert(type == TYPE_NONE or type == TYPE_COLOR);

    type = TYPE_COLOR;
    value_color = QColor(color.r, color.g, color.b);
}
