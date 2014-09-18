#include "OpenGLRenderer.h"

#include OPENGL_FUNCTIONS_INCLUDE
#include "CameraDefinition.h"
#include "OpenGLSharedState.h"
#include "OpenGLSkybox.h"
#include "OpenGLWater.h"
#include "OpenGLTerrain.h"
#include "Scenery.h"
#include "LightingManager.h"
#include "Logs.h"

#include "GL/glu.h" // TEMP

OpenGLRenderer::OpenGLRenderer(Scenery* scenery):
    SoftwareRenderer(scenery)
{
    ready = false;
    vp_width = 1;
    vp_height = 1;

    render_quality = 3;

    functions = new OpenGLFunctions();
    shared_state = new OpenGLSharedState();

    shared_state->set("viewDistance", 300.0);
    shared_state->set("exposure", 1.2);

    skybox = new OpenGLSkybox(this);
    water = new OpenGLWater(this);
    terrain = new OpenGLTerrain(this);
}

OpenGLRenderer::~OpenGLRenderer()
{
    terrain->interrupt();
    water->interrupt();
    skybox->interrupt();

    delete skybox;
    delete water;
    delete terrain;

    delete functions;
    delete shared_state;
}

void OpenGLRenderer::initialize()
{
    ready = functions->initializeOpenGLFunctions();

    if (ready)
    {
        prepareOpenGLState();

        prepare();

        disableClouds();
        getLightingManager()->setSpecularity(false);

        skybox->initialize();
        skybox->updateScenery();

        water->initialize();
        water->updateScenery();

        terrain->initialize();
        terrain->updateScenery();

        cameraChangeEvent(getScenery()->getCamera());
    }
    else
    {
        logError("Failed to initialize OpenGL bindings");
    }
}

void OpenGLRenderer::prepareOpenGLState()
{
    if (ready)
    {
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

        functions->glEnable(GL_BLEND);
        functions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        functions->glClearColor(0.0, 0.0, 0.0, 0.0);
        functions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        functions->glViewport(0, 0, vp_width, vp_height);
    }
}

void OpenGLRenderer::setCamera(CameraDefinition *camera)
{
    camera->copy(render_camera);
    getScenery()->setCamera(camera);
    getScenery()->getCamera(camera);
    cameraChangeEvent(camera);
}

void OpenGLRenderer::resize(int width, int height)
{
    vp_width = width;
    vp_height = height;

    getScenery()->getCamera()->setRenderSize(width, height);
    render_camera->setRenderSize(width, height);

    cameraChangeEvent(getScenery()->getCamera());

    prepareOpenGLState();
}

void OpenGLRenderer::paint()
{
    if (ready)
    {
        functions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        skybox->render();
        terrain->render();
        water->render();

        int error_code;
        while ((error_code = glGetError()) != GL_NO_ERROR)
        {
            logWarning("[OpenGL] ERROR : %s", (const char*)gluErrorString(error_code));
        }
    }
}

void OpenGLRenderer::cameraChangeEvent(CameraDefinition *camera)
{
    // Get camera info
    Vector3 location = camera->getLocation();
    Vector3 target = camera->getTarget();
    Vector3 up = camera->getUpVector();
    CameraPerspective perspective = camera->getPerspective();

    // Compute matrix
    QMatrix4x4 transform;
    transform.setToIdentity();
    transform.lookAt(QVector3D(location.x, location.y, location.z),
                  QVector3D(target.x, target.y, target.z),
                  QVector3D(up.x, up.y, up.z));

    QMatrix4x4 projection;
    projection.setToIdentity();
    projection.perspective(perspective.yfov * 180.0 / M_PI, perspective.xratio, perspective.znear, perspective.zfar);

    // Set in shaders
    shared_state->set("cameraLocation", location);
    shared_state->set("viewMatrix", projection * transform);
}

double OpenGLRenderer::getPrecision(const Vector3 &)
{
    return 0.0000001;
}

Color OpenGLRenderer::applyMediumTraversal(Vector3, Color color)
{
    return color;
}
