#include "OpenGLRenderer.h"

#include <QMatrix4x4>
#include "Maths.h"
#include "OpenGLFunctions.h"
#include "CameraDefinition.h"
#include "OpenGLSharedState.h"
#include "OpenGLSkybox.h"
#include "OpenGLWater.h"
#include "OpenGLTerrain.h"
#include "OpenGLVegetation.h"
#include "CloudsRenderer.h"
#include "VegetationRenderer.h"
#include "Color.h"
#include "Scenery.h"
#include "LightingManager.h"
#include "GodRaysSampler.h"
#include "Logs.h"
#include "Vector3.h"
#include "NoiseFunctionSimplex.h"

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
    shared_state->set("exposure", to_float(1.2));

    parts.push_back(skybox = new OpenGLSkybox(this));
    parts.push_back(water = new OpenGLWater(this));
    parts.push_back(terrain = new OpenGLTerrain(this));
    parts.push_back(vegetation = new OpenGLVegetation(this));
}

OpenGLRenderer::~OpenGLRenderer() {
    for (auto part : parts) {
        part->interrupt();
    }

    delete mouse_projected;
    delete view_matrix;

    for (auto part : parts) {
        delete part;
    }

    delete functions;
    delete shared_state;
}

void OpenGLRenderer::prepare() {
    SoftwareRenderer::prepare();

    getCloudsRenderer()->setEnabled(false);
    getLightingManager()->setSpecularity(false);
    getGodRaysSampler()->setEnabled(false);
    getVegetationRenderer()->setEnabled(false);
}

void OpenGLRenderer::checkForErrors(const string &domain) {
    unsigned int error_code;
    while ((error_code = functions->glGetError()) != GL_NO_ERROR) {
        Logs::warning("OpenGL") << "Error in " << domain << " : " << error_code << endl;
    }
}

void OpenGLRenderer::destroy() {
    shared_state->destroy(functions);

    for (auto part : parts) {
        part->destroy();
    }

    checkForErrors("destroy");
}

void OpenGLRenderer::initialize() {
    bool init = functions->initializeOpenGLFunctions();

    if (init) {
        Logs::debug("OpenGL") << "Renderer started (version " << functions->glGetString(GL_VERSION) << ", glsl version "
                              << functions->glGetString(GL_SHADING_LANGUAGE_VERSION) << ")" << endl;

        prepareOpenGLState();

        prepare();

        for (auto part : parts) {
            part->initialize();
            part->updateScenery();
        }

        // Common state values
        shared_state->set("simplexSampler", NoiseFunctionSimplex::getNormalTexture(), true, true);

        cameraChangeEvent(render_camera);

        checkForErrors("initialize");
        ready = true;
    } else {
        Logs::error("OpenGL") << "Failed to initialize api bindings" << endl;
    }
}

void OpenGLRenderer::prepareOpenGLState(bool clear) {
    if (ready) {
        functions->glFrontFace(GL_CCW);
        functions->glCullFace(GL_BACK);
        functions->glEnable(GL_CULL_FACE);

        functions->glDepthFunc(GL_LESS);
        functions->glDepthMask(GL_TRUE);
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

        for (auto part : parts) {
            part->render();
            checkForErrors(part->getName());
        }

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
        for (auto part : parts) {
            part->updateScenery();
        }

        cameraChangeEvent(render_camera);
    }
}

void OpenGLRenderer::pause() {
    if (not paused) {
        paused = true;
        for (auto part : parts) {
            part->pause();
        }
    }
}

void OpenGLRenderer::resume() {
    if (paused) {
        for (auto part : parts) {
            part->resume();
        }
        paused = false;
    }
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
    // TODO Switch to CameraDefinition raw transformation matrix (currently produces strange results)
    QMatrix4x4 transform;
    transform.setToIdentity();
    transform.lookAt(QVector3D(location.x, location.y, location.z), QVector3D(target.x, target.y, target.z),
                     QVector3D(up.x, up.y, up.z));

    QMatrix4x4 projection;
    projection.setToIdentity();
    projection.perspective(perspective.yfov * 180.0 / Maths::PI, perspective.xratio, perspective.znear,
                           perspective.zfar);

    *view_matrix = projection *transform;

    // Set in shaders
    shared_state->set("cameraLocation", location);
    float *matdata = view_matrix->transposed().data();
    shared_state->set("viewMatrix", Matrix4(matdata[0], matdata[1], matdata[2], matdata[3], matdata[4], matdata[5],
                                            matdata[6], matdata[7], matdata[8], matdata[9], matdata[10], matdata[11],
                                            matdata[12], matdata[13], matdata[14], matdata[15]));

    // Broadcast to parts
    vegetation->cameraChanged(camera);
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

    QVector4D located(to_float(mouse_x / render_camera->getWidth()), to_float(mouse_y / render_camera->getHeight()), z,
                      1.0);
    QVector4D unprojected = view_matrix->inverted() * 2.0 * (located - QVector4D(0.5, 0.5, 0.5, 0.5));
    *mouse_projected = Vector3(unprojected.x() / unprojected.w(), unprojected.y() / unprojected.w(),
                               unprojected.z() / unprojected.w());

    shared_state->set("mouseProjection", *mouse_projected);
}
