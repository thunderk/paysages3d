#include "OpenGLVariable.h"

#include OPENGL_FUNCTIONS_INCLUDE
#include <QOpenGLShaderProgram>
#include <cassert>
#include "OpenGLRenderer.h"
#include "OpenGLShaderProgram.h"
#include "Vector3.h"
#include "Matrix4.h"
#include "Color.h"
#include "Texture2D.h"
#include "Texture3D.h"
#include "Texture4D.h"

OpenGLVariable::OpenGLVariable(const std::string &name) : name(name) {
    type = TYPE_NONE;
    texture_toupload = false;
    texture_id = 0;
}

void OpenGLVariable::apply(OpenGLShaderProgram *program, int &texture_unit) {
    QOpenGLShaderProgram *pr = program->getProgram();
    OpenGLFunctions *functions = program->getRenderer()->getOpenGlFunctions();

    if (texture_toupload) {
        uploadTexture(program->getRenderer());
        texture_toupload = false;
    }

    switch (type) {
    case TYPE_FLOAT:
        pr->setUniformValue(name.c_str(), value_float);
        break;
    case TYPE_COLOR:
        pr->setUniformValue(name.c_str(), value_color);
        break;
    case TYPE_VECTOR3:
        pr->setUniformValue(name.c_str(), value_vector3);
        break;
    case TYPE_MATRIX4:
        pr->setUniformValue(name.c_str(), value_matrix4);
        break;
    case TYPE_TEXTURE_2D:
        functions->glActiveTexture(GL_TEXTURE0 + texture_unit);
        functions->glBindTexture(GL_TEXTURE_2D, texture_id);
        pr->setUniformValue(name.c_str(), texture_unit);
        texture_unit++;
        break;
    case TYPE_TEXTURE_3D:
    case TYPE_TEXTURE_4D:
        functions->glActiveTexture(GL_TEXTURE0 + texture_unit);
        functions->glBindTexture(GL_TEXTURE_3D, texture_id);
        pr->setUniformValue(name.c_str(), texture_unit);
        texture_unit++;
        break;
    case TYPE_NONE:
        break;
    }
}

void OpenGLVariable::set(const Texture2D *texture, bool repeat, bool color) {
    assert(type == TYPE_NONE or type == TYPE_TEXTURE_2D);

    type = TYPE_TEXTURE_2D;
    value_tex2d = texture;
    texture_toupload = true;
    texture_repeat = repeat;
    texture_color = color;
}

void OpenGLVariable::set(const Texture3D *texture, bool repeat, bool color) {
    assert(type == TYPE_NONE or type == TYPE_TEXTURE_3D);

    type = TYPE_TEXTURE_3D;
    value_tex3d = texture;
    texture_toupload = true;
    texture_repeat = repeat;
    texture_color = color;
}

void OpenGLVariable::set(const Texture4D *texture, bool repeat, bool color) {
    assert(type == TYPE_NONE or type == TYPE_TEXTURE_4D);

    type = TYPE_TEXTURE_4D;
    value_tex4d = texture;
    texture_toupload = true;
    texture_repeat = repeat;
    texture_color = color;
}

void OpenGLVariable::set(float value) {
    assert(type == TYPE_NONE or type == TYPE_FLOAT);

    type = TYPE_FLOAT;
    value_float = value;
}

void OpenGLVariable::set(const Vector3 &vector) {
    set(QVector3D(vector.x, vector.y, vector.z));
}

void OpenGLVariable::set(const QVector3D &vector) {
    assert(type == TYPE_NONE or type == TYPE_VECTOR3);

    type = TYPE_VECTOR3;
    value_vector3 = vector;
}

void OpenGLVariable::set(const Matrix4 &matrix) {
    set(matrix.toQMatrix());
}

void OpenGLVariable::set(const QMatrix4x4 &matrix) {
    assert(type == TYPE_NONE or type == TYPE_MATRIX4);

    type = TYPE_MATRIX4;
    value_matrix4 = matrix;
}

void OpenGLVariable::set(const Color &color) {
    assert(type == TYPE_NONE or type == TYPE_COLOR);

    type = TYPE_COLOR;
    value_color = QColor::fromRgbF(color.r, color.g, color.b);
}

void OpenGLVariable::uploadTexture(OpenGLRenderer *renderer) {
    OpenGLFunctions *functions = renderer->getOpenGlFunctions();

    assert(type == TYPE_TEXTURE_2D or type == TYPE_TEXTURE_3D or type == TYPE_TEXTURE_4D);

    if (texture_id == 0) {
        GLuint texid;
        functions->glGenTextures(1, &texid);
        texture_id = texid;
    }

    GLenum textype = (type == TYPE_TEXTURE_2D) ? GL_TEXTURE_2D : GL_TEXTURE_3D;

    functions->glBindTexture(textype, texture_id);
    functions->glTexParameteri(textype, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    functions->glTexParameteri(textype, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    functions->glTexParameteri(textype, GL_TEXTURE_WRAP_S, texture_repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    functions->glTexParameteri(textype, GL_TEXTURE_WRAP_T, texture_repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    if (textype == GL_TEXTURE_3D) {
        functions->glTexParameteri(textype, GL_TEXTURE_WRAP_R, texture_repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    }

    int dest_format = texture_color ? GL_RGBA : GL_RED;

    if (type == TYPE_TEXTURE_2D) {
        int sx, sy;
        value_tex2d->getSize(&sx, &sy);
        float *pixels = new float[sx * sy * 4];
        for (int x = 0; x < sx; x++) {
            for (int y = 0; y < sy; y++) {
                float *pixel = pixels + (y * sx + x) * 4;
                Color col = value_tex2d->getPixel(x, y);
                pixel[0] = (float)col.r;
                pixel[1] = (float)col.g;
                pixel[2] = (float)col.b;
                pixel[3] = (float)col.a;
            }
        }

        functions->glTexImage2D(GL_TEXTURE_2D, 0, dest_format, sx, sy, 0, GL_RGBA, GL_FLOAT, pixels);
        delete[] pixels;
    } else if (type == TYPE_TEXTURE_3D) {
        int sx, sy, sz;
        value_tex3d->getSize(&sx, &sy, &sz);
        float *pixels = new float[sx * sy * sz * 4];
        for (int x = 0; x < sx; x++) {
            for (int y = 0; y < sy; y++) {
                for (int z = 0; z < sz; z++) {
                    float *pixel = pixels + (z * (sx * sy) + y * sx + x) * 4;
                    Color col = value_tex3d->getPixel(x, y, z);
                    pixel[0] = (float)col.r;
                    pixel[1] = (float)col.g;
                    pixel[2] = (float)col.b;
                    pixel[3] = (float)col.a;
                }
            }
        }

        functions->glTexImage3D(GL_TEXTURE_3D, 0, dest_format, sx, sy, sz, 0, GL_RGBA, GL_FLOAT, pixels);
        delete[] pixels;
    } else {
        int sx, sy, sz, sw;
        value_tex4d->getSize(&sx, &sy, &sz, &sw);
        float *pixels = new float[sx * sy * sz * sw * 4];
        for (int x = 0; x < sx; x++) {
            for (int y = 0; y < sy; y++) {
                for (int z = 0; z < sz; z++) {
                    for (int w = 0; w < sw; w++) {
                        float *pixel = pixels + (w * (sx * sy * sz) + z * (sx * sy) + y * sx + x) * 4;
                        Color col = value_tex4d->getPixel(x, y, z, w);
                        pixel[0] = (float)col.r;
                        pixel[1] = (float)col.g;
                        pixel[2] = (float)col.b;
                        pixel[3] = (float)col.a;
                    }
                }
            }
        }

        functions->glTexImage3D(GL_TEXTURE_3D, 0, dest_format, sx, sy, sz * sw, 0, GL_RGBA, GL_FLOAT, pixels);
        delete[] pixels;
    }
}
