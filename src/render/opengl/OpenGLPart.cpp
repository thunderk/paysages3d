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

void OpenGLPart::postInitialize()
{
    QMapIterator<QString, OpenGLShaderProgram*> i(shaders);
    while (i.hasNext())
    {
        i.next();
        i.value()->compile();
    }
}

void OpenGLPart::updateCamera(CameraDefinition* camera)
{
    // Get camera info
    Vector3 location = camera->getLocation();
    Vector3 target = camera->getTarget();
    Vector3 up = camera->getUpVector();
    CameraPerspective perspective = camera->getPerspective();

    QVector3D vlocation(location.x, location.y, location.z);

    // Compute matrix
    QMatrix4x4 transform;
    transform.setToIdentity();
    transform.lookAt(vlocation,
                  QVector3D(target.x, target.y, target.z),
                  QVector3D(up.x, up.y, up.z));

    QMatrix4x4 projection;
    projection.setToIdentity();
    projection.perspective(perspective.yfov * 180.0 / M_PI, perspective.xratio, perspective.znear, perspective.zfar);

    // Set in shaders
    QMapIterator<QString, OpenGLShaderProgram*> i(shaders);
    while (i.hasNext())
    {
        i.next();
        i.value()->updateCamera(vlocation, projection * transform);
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
