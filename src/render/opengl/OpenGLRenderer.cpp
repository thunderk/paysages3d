#include "OpenGLRenderer.h"

#include "OpenGLFunctions.h"
#include "CameraDefinition.h"
#include "OpenGLSharedState.h"
#include "OpenGLSkybox.h"
#include "OpenGLWater.h"
#include "OpenGLTerrain.h"
#include "CloudsRenderer.h"
#include "Color.h"
#include "Scenery.h"
#include "LightingManager.h"
#include "GodRaysSampler.h"
#include "Logs.h"
#include "Vector3.h"

OpenGLRenderer::OpenGLRenderer(Scenery *scenery) : SoftwareRenderer(scenery) {
    ready = false;
    paused = false;
    displayed = false;
    stopping = false;
    stopped = false;
    vp_width = 1;
    vp_height = 1;

    mouse_tracking = true;
    mouse_x = 0;
    mouse_y = 0;
    mouse_projected = new Vector3();

    view_matrix = new QMatrix4x4;

    setQuality(0.3);

    functions = new OpenGLFunctions();
    shared_state = new OpenGLSharedState();

    shared_state->set("viewDistance", 300.0);
    shared_state->set("exposure", 1.2);

    skybox = new OpenGLSkybox(this);
    water = new OpenGLWater(this);
    terrain = new OpenGLTerrain(this);
}

OpenGLRenderer::~OpenGLRenderer() {
    terrain->interrupt();
    water->interrupt();
    skybox->interrupt();

    delete mouse_projected;

    delete view_matrix;

    delete skybox;
    delete water;
    delete terrain;

    delete functions;
    delete shared_state;
}

void OpenGLRenderer::checkForErrors(const std::string &domain) {
    int error_code;
    while ((error_code = functions->glGetError()) != GL_NO_ERROR) {
        Logs::warning() << "[OpenGL] Error in " << domain << " : " << error_code << std::endl;
    }
}

void OpenGLRenderer::destroy() {
    shared_state->destroy(functions);

    skybox->destroy();
    terrain->destroy();
    water->destroy();

    checkForErrors("stopping");
}

void OpenGLRenderer::initialize() {
    ready = functions->initializeOpenGLFunctions();

    if (ready) {
        Logs::debug() << "[OpenGL] renderer started (version " << functions->glGetString(GL_VERSION)
                      << ", glsl version " << functions->glGetString(GL_SHADING_LANGUAGE_VERSION) << ")" << std::endl;

        prepareOpenGLState();

        prepare();

        getCloudsRenderer()->setEnabled(false);
        getLightingManager()->setSpecularity(false);
        getGodRaysSampler()->setEnabled(false);

        skybox->initialize();
        skybox->updateScenery();

        water->initialize();
        water->updateScenery();

        terrain->initialize();
        terrain->updateScenery();

        cameraChangeEvent(render_camera);

        checkForErrors("initialize");
    } else {
        Logs::error() << "[OpenGL] Failed to initialize api bindings" << std::endl;
    }
}

void OpenGLRenderer::prepareOpenGLState(bool clear) {
    if (ready) {
        functions->glFrontFace(GL_CCW);
        functions->glCullFace(GL_BACK);
        functions->glEnable(GL_CULL_FACE);

        functions->glDepthFunc(GL_LESS);
        functions->glDepthMask(1);
        functions->glEnable(GL_DEPTH_TEST);

        functions->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        /*functions->glEnable(GL_LINE_SMOOTH);
        functions->glLineWidth(1.0);*/

        functions->glEnable(GL_BLEND);
        functions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        functions->glClearColor(0.0, 0.0, 0.0, 0.0);
        if (clear) {
            functions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        functions->glViewport(0, 0, vp_width, vp_height);
    }
}

void OpenGLRenderer::setCamera(CameraDefinition *camera) {
    camera->copy(render_camera);
    getScenery()->keepCameraAboveGround(render_camera);
    render_camera->setRenderSize(vp_width, vp_height);
    cameraChangeEvent(render_camera);
}

void OpenGLRenderer::resize(int width, int height) {
    if (ready) {
        vp_width = width;
        vp_height = height;

        render_camera->setRenderSize(width, height);
        cameraChangeEvent(render_camera);

        prepareOpenGLState();

        checkForErrors("resize");
    }
}

void OpenGLRenderer::paint(bool clear) {
    if (stopping) {
        if (not stopped) {
            destroy();
            stopped = true;
        }
    } else if (ready and not paused) {
        checkForErrors("before_paint");
        if (clear) {
            functions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        skybox->render();
        checkForErrors("skybox");

        terrain->render();
        checkForErrors("terrain");

        water->render();
        checkForErrors("water");

        if (mouse_tracking) {
            updateMouseProjection();
            checkForErrors("mouse_tracking");
        }

        displayed = true;
    }
}

bool OpenGLRenderer::stop() {
    stopping = true;
    return stopped;
}

void OpenGLRenderer::reset() {
    if (ready) {
        skybox->updateScenery();
        water->updateScenery();
        terrain->updateScenery();

        cameraChangeEvent(render_camera);
    }
}

void OpenGLRenderer::pause() {
    paused = true;
    terrain->pause();
}

void OpenGLRenderer::resume() {
    paused = false;
    terrain->resume();
}

void OpenGLRenderer::setMouseLocation(int x, int y) {
    mouse_x = x;
    mouse_y = y;
}

const Vector3 &OpenGLRenderer::getMouseProjection() {
    return *mouse_projected;
}

void OpenGLRenderer::cameraChangeEvent(CameraDefinition *camera) {
    // Get camera info
    Vector3 location = camera->getLocation();
    Vector3 target = camera->getTarget();
    Vector3 up = camera->getUpVector();
    CameraPerspective perspective = camera->getPerspective();

    // Compute matrix
    QMatrix4x4 transform;
    transform.setToIdentity();
    transform.lookAt(QVector3D(location.x, location.y, location.z), QVector3D(target.x, target.y, target.z),
                     QVector3D(up.x, up.y, up.z));

    QMatrix4x4 projection;
    projection.setToIdentity();
    projection.perspective(perspective.yfov * 180.0 / M_PI, perspective.xratio, perspective.znear, perspective.zfar);

    *view_matrix = projection *transform;

    // Set in shaders
    shared_state->set("cameraLocation", location);
    shared_state->set("viewMatrix", *view_matrix);
}

double OpenGLRenderer::getPrecision(const Vector3 &) {
    return 0.0000001;
}

Color OpenGLRenderer::applyMediumTraversal(const Vector3 &, const Color &color) {
    return color;
}

void OpenGLRenderer::updateMouseProjection() {
    GLfloat z;
    functions->glReadPixels(mouse_x, mouse_y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);

    QVector4D located(mouse_x / render_camera->getWidth(), mouse_y / render_camera->getHeight(), z, 1.0);
    QVector4D unprojected = view_matrix->inverted() * 2.0 * (located - QVector4D(0.5, 0.5, 0.5, 0.5));
    *mouse_projected = Vector3(unprojected.x() / unprojected.w(), unprojected.y() / unprojected.w(),
                               unprojected.z() / unprojected.w());

    shared_state->set("mouseProjection", *mouse_projected);
}
