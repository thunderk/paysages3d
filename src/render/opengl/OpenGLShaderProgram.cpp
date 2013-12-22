#include "OpenGLShaderProgram.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_3_2_Core>
#include <QDir>
#include "OpenGLRenderer.h"
#include "OpenGLSharedState.h"
#include "Texture2D.h"
#include "Texture3D.h"
#include "Texture4D.h"
#include "Color.h"

OpenGLShaderProgram::OpenGLShaderProgram(QString name, OpenGLRenderer* renderer):
    renderer(renderer), name(name)
{
    program = new QOpenGLShaderProgram();
    functions = renderer->getOpenGlFunctions();
}

OpenGLShaderProgram::~OpenGLShaderProgram()
{
    delete program;
}

void OpenGLShaderProgram::addVertexSource(QString path)
{
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, QString(":/shaders/%1.vert").arg(path));
}

void OpenGLShaderProgram::addFragmentSource(QString path)
{
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, QString(":/shaders/%1.frag").arg(path));
}

void OpenGLShaderProgram::compile()
{
    if (not program->link())
    {
        qWarning() << "Error while compiling shader " << name << "\n" << program->log() << "\n";
    }
    else
    {
        qDebug() << "Shader " << name << " compilation output:\n" << program->log() << "\n";
    }
}

void OpenGLShaderProgram::bind()
{
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
