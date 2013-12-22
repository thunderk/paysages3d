#include "OpenGLPart.h"

#include <QDir>
#include <cmath>
#include "OpenGLRenderer.h"
#include "OpenGLShaderProgram.h"
#include "CameraDefinition.h"
#include "AtmosphereDefinition.h"
#include "AtmosphereRenderer.h"
#include "Scenery.h"

OpenGLPart::OpenGLPart(OpenGLRenderer* renderer):
    renderer(renderer)
{
}

OpenGLPart::~OpenGLPart()
{
    QMapIterator<QString, OpenGLShaderProgram*> i(shaders);
    while (i.hasNext())
    {
        i.next();
        delete i.value();
    }
}

OpenGLShaderProgram* OpenGLPart::createShader(QString name)
{
    OpenGLShaderProgram* program = new OpenGLShaderProgram(name, renderer);

    if (!shaders.contains(name))
    {
        shaders[name] = program;
        return program;
    }
    else
    {
        return 0;
    }
}

void OpenGLPart::updateScenery(bool onlyCommon)
{
    // Let subclass do its own collecting
    if (not onlyCommon)
    {
        update();
    }
}
