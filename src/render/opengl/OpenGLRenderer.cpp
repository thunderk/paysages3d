#include "OpenGLRenderer.h"

#include <QOpenGLFunctions_3_2_Core>
#include "CameraDefinition.h"
#include "OpenGLSkybox.h"
#include "OpenGLWater.h"

OpenGLRenderer::OpenGLRenderer(Scenery* scenery):
    SoftwareRenderer(scenery)
{
    functions = new QOpenGLFunctions_3_2_Core();
    skybox = new OpenGLSkybox(this);
    water = new OpenGLWater(this);
}

OpenGLRenderer::~OpenGLRenderer()
{
    delete skybox;
    delete water;
    delete functions;
}

void OpenGLRenderer::initialize()
{
    // TODO Check return value
    functions->initializeOpenGLFunctions();

    functions->glClearColor(0.0, 0.0, 0.0, 0.0);

    functions->glDisable(GL_LIGHTING);

    functions->glFrontFace(GL_CCW);
    functions->glCullFace(GL_BACK);
    functions->glEnable(GL_CULL_FACE);

    functions->glDepthFunc(GL_LESS);
    functions->glDepthMask(1);
    functions->glEnable(GL_DEPTH_TEST);

    functions->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    functions->glEnable(GL_LINE_SMOOTH);
    functions->glLineWidth(1.0);

    functions->glDisable(GL_FOG);

    functions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    prepare();

    skybox->initialize();
    skybox->updateScenery();

    water->initialize();
    water->updateScenery();
}

void OpenGLRenderer::resize(int width, int height)
{
    functions->glViewport(0, 0, width, height);
}

void OpenGLRenderer::paint()
{
    functions->glClearColor(0.0, 0.0, 0.0, 0.0);
    functions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    skybox->render();
    water->render();
}

void OpenGLRenderer::cameraChangeEvent(CameraDefinition *camera)
{
    skybox->updateCamera(camera);
    water->updateCamera(camera);
}

double OpenGLRenderer::getPrecision(const Vector3 &)
{
    return 0.0000001;
}

Color OpenGLRenderer::applyMediumTraversal(Vector3, Color color)
{
    return color;
}
