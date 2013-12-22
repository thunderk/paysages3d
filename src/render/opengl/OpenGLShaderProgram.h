#ifndef OPENGLSHADERPROGRAM_H
#define OPENGLSHADERPROGRAM_H

#include "opengl_global.h"

#include <QString>
#include <QMatrix4x4>
#include <QColor>
#include <QMap>
#include <QPair>

class QOpenGLShaderProgram;
class QOpenGLFunctions_3_2_Core;

namespace paysages {
namespace opengl {

class OPENGLSHARED_EXPORT OpenGLShaderProgram
{
public:
    OpenGLShaderProgram(QString name, OpenGLRenderer* renderer);
    ~OpenGLShaderProgram();

    void addVertexSource(QString path);
    void addFragmentSource(QString path);
    void compile();

    void addTexture(QString sampler_name, Texture2D* texture);
    void addTexture(QString sampler_name, Texture3D* texture);
    void addTexture(QString sampler_name, Texture4D* texture);

    void drawTriangles(float* vertices, int triangle_count);
    void drawTriangleStrip(float* vertices, int vertex_count);

protected:
    inline QOpenGLShaderProgram* getProgram() const {return program;}
    friend class OpenGLVariable;

private:
    void bind();
    void release();

    OpenGLRenderer* renderer;

    QString name;
    QOpenGLShaderProgram* program;
    QOpenGLFunctions_3_2_Core* functions;

    QMap<QString, QPair<int, unsigned int> > textures;
};

}
}

#endif // OPENGLSHADERPROGRAM_H
