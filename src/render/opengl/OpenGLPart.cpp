#include "OpenGLPart.h"

#include "OpenGLRenderer.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLVertexArray.h"

OpenGLPart::OpenGLPart(OpenGLRenderer *renderer) : renderer(renderer) {
}

OpenGLPart::~OpenGLPart() {
    for (auto &pair: shaders) {
        delete pair.second;
    }
    for (auto &array: arrays) {
        delete array;
    }
}

void OpenGLPart::interrupt() {
}

OpenGLShaderProgram *OpenGLPart::createShader(const std::string &name) {
    OpenGLShaderProgram *program = new OpenGLShaderProgram(name, renderer);

    if (shaders.find(name) == shaders.end()) {
        shaders[name] = program;
        return program;
    } else {
        return 0;
    }
}

OpenGLVertexArray *OpenGLPart::createVertexArray(bool has_uv, bool strip)
{
    OpenGLVertexArray *result = new OpenGLVertexArray(has_uv, strip);
    arrays.push_back(result);
    return result;
}

void OpenGLPart::updateScenery(bool onlyCommon) {
    // Let subclass do its own collecting
    if (not onlyCommon) {
        update();
    }
}

Scenery *OpenGLPart::getScenery() const {
    return renderer->getScenery();
}

OpenGLFunctions *OpenGLPart::getOpenGlFunctions() const {
    return renderer->getOpenGlFunctions();
}
