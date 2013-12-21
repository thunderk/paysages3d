#ifndef OPENGLSHADERPROGRAM_H
#define OPENGLSHADERPROGRAM_H

#include "opengl_global.h"

#include <QString>
#include <QMatrix4x4>
#include <QColor>
#include <QMap>
#include <QPair>

class QOpenGLShaderProgram;
class QOpenGLFunctions;

namespace paysages {
namespace opengl {

class OPENGLSHARED_EXPORT OpenGLShaderProgram
{
public:
    OpenGLShaderProgram(QString name, QOpenGLFunctions* functions);
    ~OpenGLShaderProgram();

    void addVertexSource(QString path);
    void addFragmentSource(QString path);
    void compile();

    void updateCamera(const QVector3D& location, const QMatrix4x4& view);
    void updateWaterHeight(double height);
    void updateSun(const QVector3D& direction, const QColor& color);

    void addTexture(QString sampler_name, Texture2D* texture);
    void addTexture(QString sampler_name, Texture3D* texture);
    void addTexture(QString sampler_name, Texture4D* texture);

    void drawTriangles(float* vertices, int triangle_count);
    void drawTriangleStrip(float* vertices, int vertex_count);

private:
    void bind();
    void release();

    QMatrix4x4 view;
    QVector3D camera_location;

    float water_height;

    QVector3D sun_direction;
    QColor sun_color;

    QString name;
    QOpenGLShaderProgram* program;
    QOpenGLFunctions* functions;

    QMap<QString, QPair<int, unsigned int> > textures;
};

}
}

#endif // OPENGLSHADERPROGRAM_H
