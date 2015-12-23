#include "OpenGLVariable.h"

#include <cassert>
#include <QOpenGLShaderProgram>
#include <QImage>
#include "Logs.h"
#include "OpenGLFunctions.h"
#include "OpenGLRenderer.h"
#include "OpenGLShaderProgram.h"
#include "Vector3.h"
#include "Matrix4.h"
#include "Color.h"
#include "Texture2D.h"
#include "Texture3D.h"
#include "Texture4D.h"

typedef enum {
    TYPE_NONE,
    TYPE_TEXTURE_2D,
    TYPE_TEXTURE_3D,
    TYPE_TEXTURE_4D,
    TYPE_INTEGER,
    TYPE_FLOAT,
    TYPE_VECTOR3,
    TYPE_MATRIX4,
    TYPE_COLOR
} OpenGLVariableType;

class OpenGLVariable::pimpl {
  public:
    pimpl(const string &name) : name(name) {
        type = TYPE_NONE;
        texture_toupload = false;
        texture_id = 0;
    }

    string name;
    OpenGLVariableType type;

    int value_int;
    float value_float;
    unique_ptr<Color> value_color;
    unique_ptr<Vector3> value_vector3;
    unique_ptr<float[]> value_array_float;

    int texture_size_x;
    int texture_size_y;
    int texture_size_z;
    bool texture_toupload;
    bool texture_repeat;
    bool texture_color;
    unsigned int texture_id;
};

OpenGLVariable::OpenGLVariable(const string &name) : impl(new pimpl(name)) {
}

OpenGLVariable::~OpenGLVariable() {
    if (impl->texture_id) {
        Logs::warning("OpenGL") << "Texture ID " << impl->texture_id << " not freed in variable " << impl->name << endl;
    }
}

void OpenGLVariable::apply(OpenGLShaderProgram *program, unsigned int &texture_unit) {
    assert(program->isBound());

    OpenGLFunctions *functions = program->getRenderer()->getOpenGlFunctions();

    if (impl->texture_toupload) {
        uploadTexture(functions);
        impl->texture_toupload = false;
    }

    int loc = functions->glGetUniformLocation(program->getId(), impl->name.c_str());
    if (loc >= 0) {
        switch (impl->type) {
        case TYPE_INTEGER:
            functions->glUniform1i(loc, impl->value_int);
            break;
        case TYPE_FLOAT:
            functions->glUniform1f(loc, impl->value_float);
            break;
        case TYPE_COLOR:
            functions->glUniform4f(loc, to_float(impl->value_color->r), to_float(impl->value_color->g),
                                   to_float(impl->value_color->b), to_float(impl->value_color->a));
            break;
        case TYPE_VECTOR3:
            functions->glUniform3f(loc, to_float(impl->value_vector3->x), to_float(impl->value_vector3->y),
                                   to_float(impl->value_vector3->z));
            break;
        case TYPE_MATRIX4:
            functions->glUniformMatrix4fv(loc, 1, true, impl->value_array_float.get());
            break;
        case TYPE_TEXTURE_2D:
            functions->glActiveTexture(GL_TEXTURE0 + texture_unit);
            functions->glBindTexture(GL_TEXTURE_2D, impl->texture_id);
            functions->glUniform1i(loc, static_cast<int>(texture_unit));
            texture_unit++;
            break;
        case TYPE_TEXTURE_3D:
        case TYPE_TEXTURE_4D:
            functions->glActiveTexture(GL_TEXTURE0 + texture_unit);
            functions->glBindTexture(GL_TEXTURE_3D, impl->texture_id);
            functions->glUniform1i(loc, static_cast<int>(texture_unit));
            texture_unit++;
            break;
        case TYPE_NONE:
            break;
        }
    }
}

void OpenGLVariable::destroy(OpenGLFunctions *functions) {
    if (impl->texture_id) {
        functions->glDeleteTextures(1, &(impl->texture_id));
        impl->texture_id = 0;
    }
}

void OpenGLVariable::set(const Texture2D *texture, bool repeat, bool color) {
    impl->type = TYPE_TEXTURE_2D;

    int sx, sy;
    texture->getSize(&sx, &sy);
    float *pixels = new float[to_size(sx * sy * 4)];
    for (int x = 0; x < sx; x++) {
        for (int y = 0; y < sy; y++) {
            float *pixel = pixels + (y * sx + x) * 4;
            Color col = texture->getPixel(x, y);
            pixel[0] = to_float(col.r);
            pixel[1] = to_float(col.g);
            pixel[2] = to_float(col.b);
            pixel[3] = to_float(col.a);
        }
    }

    impl->value_array_float = unique_ptr<float[]>(pixels);

    impl->texture_size_x = sx;
    impl->texture_size_y = sy;
    impl->texture_size_z = 0;

    impl->texture_toupload = true;
    impl->texture_repeat = repeat;
    impl->texture_color = color;
}

void OpenGLVariable::set(const QImage &texture, bool repeat, bool color) {
    impl->type = TYPE_TEXTURE_2D;

    int sx = texture.width(), sy = texture.height();
    float *pixels = new float[to_size(sx * sy * 4)];
    for (int x = 0; x < sx; x++) {
        for (int y = 0; y < sy; y++) {
            float *pixel = pixels + (y * sx + x) * 4;
            Color col = Color::from32BitRGBA(texture.pixel(x, y));
            pixel[0] = to_float(col.r);
            pixel[1] = to_float(col.g);
            pixel[2] = to_float(col.b);
            pixel[3] = to_float(col.a);
        }
    }

    impl->value_array_float = unique_ptr<float[]>(pixels);

    impl->texture_size_x = sx;
    impl->texture_size_y = sy;
    impl->texture_size_z = 0;

    impl->texture_toupload = true;
    impl->texture_repeat = repeat;
    impl->texture_color = color;
}

void OpenGLVariable::set(const Texture3D *texture, bool repeat, bool color) {
    int sx, sy, sz;
    texture->getSize(&sx, &sy, &sz);
    float *pixels = new float[to_size(sx * sy * sz * 4)];
    for (int x = 0; x < sx; x++) {
        for (int y = 0; y < sy; y++) {
            for (int z = 0; z < sz; z++) {
                float *pixel = pixels + (z * (sx * sy) + y * sx + x) * 4;
                Color col = texture->getPixel(x, y, z);
                pixel[0] = to_float(col.r);
                pixel[1] = to_float(col.g);
                pixel[2] = to_float(col.b);
                pixel[3] = to_float(col.a);
            }
        }
    }

    impl->value_array_float = unique_ptr<float[]>(pixels);

    impl->texture_size_x = sx;
    impl->texture_size_y = sy;
    impl->texture_size_z = sz;

    impl->type = TYPE_TEXTURE_3D;
    impl->texture_toupload = true;
    impl->texture_repeat = repeat;
    impl->texture_color = color;
}

void OpenGLVariable::set(const Texture4D *texture, bool repeat, bool color) {
    int sx, sy, sz, sw;
    texture->getSize(&sx, &sy, &sz, &sw);
    float *pixels = new float[to_size(sx * sy * sz * sw * 4)];
    for (int x = 0; x < sx; x++) {
        for (int y = 0; y < sy; y++) {
            for (int z = 0; z < sz; z++) {
                for (int w = 0; w < sw; w++) {
                    float *pixel = pixels + (w * (sx * sy * sz) + z * (sx * sy) + y * sx + x) * 4;
                    Color col = texture->getPixel(x, y, z, w);
                    pixel[0] = to_float(col.r);
                    pixel[1] = to_float(col.g);
                    pixel[2] = to_float(col.b);
                    pixel[3] = to_float(col.a);
                }
            }
        }
    }

    impl->value_array_float = unique_ptr<float[]>(pixels);

    impl->texture_size_x = sx;
    impl->texture_size_y = sy;
    impl->texture_size_z = sz * sw;

    impl->type = TYPE_TEXTURE_4D;
    impl->texture_toupload = true;
    impl->texture_repeat = repeat;
    impl->texture_color = color;
}

void OpenGLVariable::set(int value) {
    impl->type = TYPE_INTEGER;
    impl->value_int = value;
}

void OpenGLVariable::set(float value) {
    impl->type = TYPE_FLOAT;
    impl->value_float = value;
}

void OpenGLVariable::set(const Vector3 &vector) {
    impl->type = TYPE_VECTOR3;
    impl->value_vector3 = make_unique<Vector3>(vector);
}

void OpenGLVariable::set(const Matrix4 &matrix) {
    impl->type = TYPE_MATRIX4;

    float *data = new float[16];
    data[0] = to_float(matrix.getA());
    data[1] = to_float(matrix.getB());
    data[2] = to_float(matrix.getC());
    data[3] = to_float(matrix.getD());
    data[4] = to_float(matrix.getE());
    data[5] = to_float(matrix.getF());
    data[6] = to_float(matrix.getG());
    data[7] = to_float(matrix.getH());
    data[8] = to_float(matrix.getI());
    data[9] = to_float(matrix.getJ());
    data[10] = to_float(matrix.getK());
    data[11] = to_float(matrix.getL());
    data[12] = to_float(matrix.getM());
    data[13] = to_float(matrix.getN());
    data[14] = to_float(matrix.getO());
    data[15] = to_float(matrix.getP());

    impl->value_array_float = unique_ptr<float[]>(data);
}

void OpenGLVariable::set(const Color &color) {
    impl->type = TYPE_COLOR;
    impl->value_color = make_unique<Color>(color);
}

int OpenGLVariable::getIntValue() const {
    return impl->value_int;
}

float OpenGLVariable::getFloatValue() const {
    return impl->value_float;
}

void OpenGLVariable::uploadTexture(OpenGLFunctions *functions) {
    assert(impl->type == TYPE_TEXTURE_2D or impl->type == TYPE_TEXTURE_3D or impl->type == TYPE_TEXTURE_4D);

    if (impl->texture_id == 0) {
        GLuint texid;
        functions->glGenTextures(1, &texid);
        impl->texture_id = texid;
    }

    GLenum textype = (impl->type == TYPE_TEXTURE_2D) ? GL_TEXTURE_2D : GL_TEXTURE_3D;

    functions->glBindTexture(textype, impl->texture_id);
    functions->glTexParameteri(textype, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    functions->glTexParameteri(textype, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    functions->glTexParameteri(textype, GL_TEXTURE_WRAP_S, impl->texture_repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    functions->glTexParameteri(textype, GL_TEXTURE_WRAP_T, impl->texture_repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    if (textype == GL_TEXTURE_3D) {
        functions->glTexParameteri(textype, GL_TEXTURE_WRAP_R, impl->texture_repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    }

    int dest_format = impl->texture_color ? GL_RGBA : GL_RED;

    if (impl->type == TYPE_TEXTURE_2D) {
        functions->glTexImage2D(GL_TEXTURE_2D, 0, dest_format, impl->texture_size_x, impl->texture_size_y, 0, GL_RGBA,
                                GL_FLOAT, impl->value_array_float.get());
    } else {
        functions->glTexImage3D(GL_TEXTURE_3D, 0, dest_format, impl->texture_size_x, impl->texture_size_y,
                                impl->texture_size_z, 0, GL_RGBA, GL_FLOAT, impl->value_array_float.get());
    }
}
