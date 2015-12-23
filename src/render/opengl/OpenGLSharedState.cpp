#include "OpenGLSharedState.h"

OpenGLSharedState::OpenGLSharedState() {
}

OpenGLSharedState::~OpenGLSharedState() {
    for (const auto &pair : variables) {
        delete pair.second;
    }
}

void OpenGLSharedState::apply(OpenGLShaderProgram *program, unsigned int &texture_unit) {
    for (const auto &pair : variables) {
        pair.second->apply(program, texture_unit);
    }
}

void OpenGLSharedState::destroy(OpenGLFunctions *functions) {
    for (const auto &pair : variables) {
        pair.second->destroy(functions);
    }
}

OpenGLVariable *OpenGLSharedState::get(const string &name) {
    OpenGLVariable *&var = variables[name];
    if (var == NULL) {
        var = new OpenGLVariable(name);
    }
    return var;
}
