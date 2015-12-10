#include "OpenGLShaderProgram.h"

#include <QOpenGLShaderProgram>
#include <QDir>
#include "OpenGLFunctions.h"
#include "OpenGLRenderer.h"
#include "OpenGLSharedState.h"
#include "OpenGLVertexArray.h"
#include "Texture2D.h"
#include "Texture3D.h"
#include "Texture4D.h"
#include "Color.h"
#include "Logs.h"

OpenGLShaderProgram::OpenGLShaderProgram(const std::string &name, OpenGLRenderer *renderer)
    : renderer(renderer), name(name) {
    program = new QOpenGLShaderProgram();
    functions = renderer->getOpenGlFunctions();
    compiled = false;
}

OpenGLShaderProgram::~OpenGLShaderProgram() {
    delete program;
}

void OpenGLShaderProgram::addVertexSource(const std::string &path) {
    QFile file(QString(":/shaders/%1.vert").arg(QString::fromStdString(path)));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        source_vertex += QString(file.readAll()).toStdString();
    } else {
        Logs::error() << "[OpenGL] Can't open vertex file " << file.fileName().toStdString() << std::endl;
    }
}

void OpenGLShaderProgram::addFragmentSource(const std::string &path) {
    QFile file(QString(":/shaders/%1.frag").arg(QString::fromStdString(path)));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        source_fragment += QString(file.readAll()).toStdString();
    } else {
        Logs::error() << "[OpenGL] Can't open fragment file " << file.fileName().toStdString() << std::endl;
    }
}

void OpenGLShaderProgram::destroy(OpenGLFunctions *functions) {
    program->removeAllShaders();
}

void OpenGLShaderProgram::compile() {
    std::string prefix = std::string("#version ") + OPENGL_GLSL_VERSION + "\n\n";

    program->addShaderFromSourceCode(QOpenGLShader::Vertex, QString::fromStdString(prefix + source_vertex));
    program->addShaderFromSourceCode(QOpenGLShader::Fragment, QString::fromStdString(prefix + source_fragment));

    program->bindAttributeLocation("vertex", 0);
    program->bindAttributeLocation("uv", 1);

    if (not program->link()) {
        Logs::warning() << "[OpenGL] Error while compiling shader " << name << std::endl
                        << program->log().toStdString() << std::endl;
    } else if (program->log().length() > 0) {
        Logs::debug() << "[OpenGL] Shader " << name << " compilation output:" << std::endl
                      << program->log().toStdString() << std::endl;
    } else {
        Logs::debug() << "[OpenGL] Shader " << name << " compiled" << std::endl;
    }
}

bool OpenGLShaderProgram::bind(OpenGLSharedState *state) {
    if (not compiled) {
        compile();
        compiled = true;
    }

    if (program->bind()) {
        int texture_unit = 0;
        renderer->getSharedState()->apply(this, texture_unit);
        if (state) {
            state->apply(this, texture_unit);
        }
        return true;
    } else {
        return false;
    }
}

void OpenGLShaderProgram::release() {
    program->release();
}

void OpenGLShaderProgram::draw(OpenGLVertexArray *vertices, OpenGLSharedState *state) {
    if (bind(state)) {
        vertices->render(functions);

        release();
    }
}
