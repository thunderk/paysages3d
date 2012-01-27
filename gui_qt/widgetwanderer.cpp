#include "widgetwanderer.h"

#include <QGLWidget>
#include <QKeyEvent>
#include "../lib_paysages/scenery.h"

WidgetWanderer::WidgetWanderer(QWidget *parent, CameraDefinition* camera):
    QGLWidget(parent)
{
    setMinimumSize(400, 300);
    setFocusPolicy(Qt::StrongFocus);

    cameraCopyDefinition(camera, &this->camera);

    this->terrain = terrainCreateDefinition();
    sceneryGetTerrain(&terrain);

    this->water = waterCreateDefinition();
    sceneryGetWater(&water);

    last_mouse_x = 0;
    last_mouse_y = 0;
}

void WidgetWanderer::keyPressEvent(QKeyEvent* event)
{
    double factor;

    if (event->modifiers() & Qt::ShiftModifier)
    {
        factor = 0.1;
    }
    else if (event->modifiers() & Qt::ControlModifier)
    {
        factor = 10.0;
    }
    else
    {
        factor = 1.0;
    }

    if (event->key() == Qt::Key_Up)
    {
        cameraStrafeForward(&camera, 0.1 * factor);
        updateGL();
    }
    else if (event->key() == Qt::Key_Down)
    {
        cameraStrafeForward(&camera, -0.1 * factor);
        updateGL();
    }
    else if (event->key() == Qt::Key_Right)
    {
        cameraStrafeRight(&camera, 0.1 * factor);
        updateGL();
    }
    else if (event->key() == Qt::Key_Left)
    {
        cameraStrafeRight(&camera, -0.1 * factor);
        updateGL();
    }
    else if (event->key() == Qt::Key_PageUp)
    {
        cameraStrafeUp(&camera, 0.1 * factor);
        updateGL();
    }
    else if (event->key() == Qt::Key_PageDown)
    {
        cameraStrafeUp(&camera, -0.1 * factor);
        updateGL();
    }
    else
    {
        QGLWidget::keyPressEvent(event);
    }
}

void WidgetWanderer::mousePressEvent(QMouseEvent* event)
{
    last_mouse_x = event->x();
    last_mouse_y = event->y();

    event->ignore();
}

void WidgetWanderer::mouseMoveEvent(QMouseEvent* event)
{
    double factor;

    if (event->modifiers() & Qt::ShiftModifier)
    {
        factor = 0.01;
    }
    else if (event->modifiers() & Qt::ControlModifier)
    {
        factor = 1.0;
    }
    else
    {
        factor = 0.1;
    }
    factor *= 0.2;

    if (event->buttons() & Qt::LeftButton)
    {
        cameraStrafeRight(&camera, (double)(last_mouse_x - event->x()) * factor);
        cameraStrafeUp(&camera, (double)(event->y() - last_mouse_y) * factor);
        updateGL();
        event->accept();
    }
    else
    {
        event->ignore();
    }

    last_mouse_x = event->x();
    last_mouse_y = event->y();
}

void WidgetWanderer::wheelEvent(QWheelEvent* event)
{
    double factor;

    if (event->modifiers() & Qt::ShiftModifier)
    {
        factor = 0.01;
    }
    else if (event->modifiers() & Qt::ControlModifier)
    {
        factor = 1.0;
    }
    else
    {
        factor = 0.1;
    }
    factor *= 0.1;

    if (event->orientation() == Qt::Vertical)
    {
        cameraStrafeForward(&camera, (double)event->delta() * factor);
        updateGL();

    }
    event->accept();
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
}

void WidgetWanderer::paintGL()
{
    double x, z, step;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera.location.x, camera.location.y, camera.location.z, camera.target.x, camera.target.y, camera.target.z, camera.up.x, camera.up.y, camera.up.z);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glDepthFunc(GL_LEQUAL);
    glDepthMask(true);
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
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glColor3f(0.0, 0.5, 0.0);
            glBegin(GL_QUADS);
            _pushTerrainVertex(&terrain, x, z);
            _pushTerrainVertex(&terrain, x, z + step);
            _pushTerrainVertex(&terrain, x + step, z + step);
            _pushTerrainVertex(&terrain, x + step, z);
            glEnd();

            glEnable(GL_LINE_SMOOTH);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glColor3f(0.0, 1.0, 0.0);
            glBegin(GL_QUADS);
            _pushTerrainVertex(&terrain, x, z);
            _pushTerrainVertex(&terrain, x, z + step);
            _pushTerrainVertex(&terrain, x + step, z + step);
            _pushTerrainVertex(&terrain, x + step, z);
            glEnd();
            glDisable(GL_LINE_SMOOTH);
        }
    }
}
