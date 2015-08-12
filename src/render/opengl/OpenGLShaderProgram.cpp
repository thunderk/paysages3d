#include "OpenGLShaderProgram.h"

#include OPENGL_FUNCTIONS_INCLUDE
#include <QOpenGLShaderProgram>
#include <QDir>
#include "OpenGLRenderer.h"
#include "OpenGLSharedState.h"
#include "Texture2D.h"
#include "Texture3D.h"
#include "Texture4D.h"
#include "Color.h"
#include "Logs.h"

OpenGLShaderProgram::OpenGLShaderProgram(QString name, OpenGLRenderer* renderer):
    renderer(renderer), name(name)
{
    program = new QOpenGLShaderProgram();
    functions = renderer->getOpenGlFunctions();
    compiled = false;
}

OpenGLShaderProgram::~OpenGLShaderProgram()
{
    delete program;
}

void OpenGLShaderProgram::addVertexSource(QString path)
{
    QFile file(QString(":/shaders/%1.vert").arg(path));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        source_vertex += QString(file.readAll()).toStdString();
    }
    else
    {
        Logs::error() << "Can't open vertex file " << file.fileName().toStdString() << std::endl;
    }
}

void OpenGLShaderProgram::addFragmentSource(QString path)
{
    QFile file(QString(":/shaders/%1.frag").arg(path));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        source_fragment += QString(file.readAll()).toStdString();
    }
    else
    {
        Logs::error() << "Can't open fragment file " << file.fileName().toStdString() << std::endl;
    }
}

void OpenGLShaderProgram::compile()
{
    program->addShaderFromSourceCode(QOpenGLShader::Vertex, QString::fromStdString(source_vertex));
    program->addShaderFromSourceCode(QOpenGLShader::Fragment, QString::fromStdString(source_fragment));

    if (not program->link())
    {
        qWarning() << "Error while compiling shader " << name << "\n" << program->log() << "\n";
    }
    else if (program->log().length() > 0)
    {
        qDebug() << "Shader " << name << " compilation output:\n" << program->log() << "\n";
    }
}

void OpenGLShaderProgram::bind()
{
    if (not compiled)
    {
        compile();
        compiled = true;
    }

    program->bind();

    int texture_unit = 0;
    renderer->getSharedState()->apply(this, texture_unit);
}

void OpenGLShaderProgram::release()
{
    program->release();
}

void OpenGLShaderProgram::drawTriangles(float* vertices, int triangle_count)
{
    bind();

    GLuint vertex = program->attributeLocation("vertex");
    program->setAttributeArray(vertex, GL_FLOAT, vertices, 3);
    program->enableAttributeArray(vertex);

    functions->glDrawArrays(GL_TRIANGLES, 0, triangle_count * 3);

    program->disableAttributeArray(vertex);

    release();
}

void OpenGLShaderProgram::drawTriangleStrip(float* vertices, int vertex_count)
{
    bind();

    GLuint vertex = program->attributeLocation("vertex");
    program->setAttributeArray(vertex, GL_FLOAT, vertices, 3);
    program->enableAttributeArray(vertex);

    functions->glDrawArrays(GL_TRIANGLE_STRIP, 0, vertex_count);

    program->disableAttributeArray(vertex);

    release();
}
