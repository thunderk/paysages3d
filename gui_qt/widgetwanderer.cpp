#include "widgetwanderer.h"

#include <QGLWidget>
#include "../lib_paysages/scenery.h"

WidgetWanderer::WidgetWanderer(QWidget *parent, CameraDefinition* camera):
    QGLWidget(parent)
{
    setMinimumSize(400, 300);

    this->camera = camera;

    this->terrain = terrainCreateDefinition();
    sceneryGetTerrain(&terrain);

    this->water = waterCreateDefinition();
    sceneryGetWater(&water);
}

void WidgetWanderer::initializeGL()
{
    glClearColor(0.4, 0.7, 0.8, 0.0);
}

void WidgetWanderer::resizeGL(int w, int h)
{
    double ratio = (double)h / (double)w;

    glViewport(0, 0, (GLint)w, (GLint)h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-2.5, 2.5, -2.5 * ratio, 2.5 * ratio, 2.0, 1000.0);
}

static inline void _pushTerrainVertex(TerrainDefinition* terrain, double x, double z)
{
    glVertex3f(x, terrainGetHeight(terrain, x, z), z);
    //glVertex3f(x, 1.0, z);
}

void WidgetWanderer::paintGL()
{
    double x, z, step;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera->location.x, camera->location.y, camera->location.z, camera->target.x, camera->target.y, camera->target.z, camera->up.x, camera->up.y, camera->up.z);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

    glLineWidth(1.0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_QUADS);
    glVertex3f(-500.0, water.height, -500.0);
    glVertex3f(-500.0, water.height, 500.0);
    glVertex3f(500.0, water.height, 500.0);
    glVertex3f(500.0, water.height, -500.0);
    glEnd();

    step = 2.0;
    for (x = -50.0; x < 50.0; x += step)
    {
        for (z = -50.0; z < 50.0; z += step)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glColor3f(0.0, 1.0, 0.0);
            glBegin(GL_QUADS);
            _pushTerrainVertex(&terrain, x, z);
            _pushTerrainVertex(&terrain, x, z + step);
            _pushTerrainVertex(&terrain, x + step, z + step);
            _pushTerrainVertex(&terrain, x + step, z);
            glEnd();

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glColor3f(0.0, 0.5, 0.0);
            glBegin(GL_QUADS);
            _pushTerrainVertex(&terrain, x, z);
            _pushTerrainVertex(&terrain, x, z + step);
            _pushTerrainVertex(&terrain, x + step, z + step);
            _pushTerrainVertex(&terrain, x + step, z);
            glEnd();
        }
    }
}
