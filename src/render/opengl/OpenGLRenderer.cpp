#include "OpenGLRenderer.h"

#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>
#include "Scenery.h"
#include "SoftwareRenderer.h"
#include "renderer.h"
#include "rendering/camera.h"

OpenGLRenderer::OpenGLRenderer(Scenery* scenery):
    scenery(scenery)
{
    renderer = new SoftwareRenderer(scenery);
    renderer->prepare();
}

OpenGLRenderer::~OpenGLRenderer()
{
    rendererDelete(renderer);
}

void OpenGLRenderer::initialize()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);

    glDisable(GL_LIGHTING);

    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glDepthFunc(GL_LESS);
    glDepthMask(1);
    glEnable(GL_DEPTH_TEST);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.0);

    glDisable(GL_FOG);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::resize(int width, int height)
{
    CameraPerspective perspective;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    perspective = cameraGetPerspective(renderer->render_camera);
    gluPerspective(perspective.yfov * 180.0 / M_PI, perspective.xratio, perspective.znear, perspective.zfar);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::paint()
{

}
