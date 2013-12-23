#ifndef OPENGLSHADERPROGRAM_H
#define OPENGLSHADERPROGRAM_H

#include "opengl_global.h"

#include <QString>

class QOpenGLShaderProgram;

namespace paysages {
namespace opengl {

class OPENGLSHARED_EXPORT OpenGLShaderProgram
{
public:
    OpenGLShaderProgram(QString name, OpenGLRenderer* renderer);
    ~OpenGLShaderProgram();

    void addVertexSource(QString path);
    void addFragmentSource(QString path);

    void drawTriangles(float* vertices, int triangle_count);
    void drawTriangleStrip(float* vertices, int vertex_count);

protected:
    inline QOpenGLShaderProgram* getProgram() const {return program;}
    inline OpenGLRenderer* getRenderer() const {return renderer;}
    friend class OpenGLVariable;

private:
    void compile();
    void bind();
    void release();

    bool compiled;

    OpenGLRenderer* renderer;

    QString name;
    QOpenGLShaderProgram* program;
    OpenGLFunctions* functions;

    std::string source_vertex;
    std::string source_fragment;
};

}
}

#endif // OPENGLSHADERPROGRAM_H
