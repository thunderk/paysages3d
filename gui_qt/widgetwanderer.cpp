#include "widgetwanderer.h"

#include <QGLWidget>
#include <QKeyEvent>
#include <QTime>
#include <math.h>
#include "../lib_paysages/scenery.h"

WidgetWanderer::WidgetWanderer(QWidget *parent, CameraDefinition* camera):
    QGLWidget(parent)
{
    setMinimumSize(400, 300);
    setFocusPolicy(Qt::StrongFocus);

    _base_camera = camera;
    cameraCopyDefinition(camera, &_current_camera);

    this->terrain = terrainCreateDefinition();
    sceneryGetTerrain(&terrain);

    this->water = waterCreateDefinition();
    sceneryGetWater(&water);
    
    average_frame_time = 0.05;
    quality = 3;
    last_mouse_x = 0;
    last_mouse_y = 0;
}

void WidgetWanderer::resetCamera()
{
    cameraCopyDefinition(_base_camera, &_current_camera);
    updateGL();
}

void WidgetWanderer::validateCamera()
{
    cameraCopyDefinition(&_current_camera, _base_camera);
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
    factor *= 3.0;

    if (event->key() == Qt::Key_Up)
    {
        cameraStrafeForward(&_current_camera, 0.1 * factor);
        updateGL();
    }
    else if (event->key() == Qt::Key_Down)
    {
        cameraStrafeForward(&_current_camera, -0.1 * factor);
        updateGL();
    }
    else if (event->key() == Qt::Key_Right)
    {
        cameraStrafeRight(&_current_camera, 0.1 * factor);
        updateGL();
    }
    else if (event->key() == Qt::Key_Left)
    {
        cameraStrafeRight(&_current_camera, -0.1 * factor);
        updateGL();
    }
    else if (event->key() == Qt::Key_PageUp)
    {
        cameraStrafeUp(&_current_camera, 0.1 * factor);
        updateGL();
    }
    else if (event->key() == Qt::Key_PageDown)
    {
        cameraStrafeUp(&_current_camera, -0.1 * factor);
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
        cameraRotateYaw(&_current_camera, (double)(event->x() - last_mouse_x) * factor * 0.1);
        cameraRotatePitch(&_current_camera, (double)(event->y() - last_mouse_y) * factor * 0.1);
        updateGL();
        event->accept();
    }
    else if (event->buttons() & Qt::RightButton)
    {
        cameraStrafeRight(&_current_camera, (double)(last_mouse_x - event->x()) * factor);
        cameraStrafeUp(&_current_camera, (double)(event->y() - last_mouse_y) * factor);
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
    factor *= 0.03;

    if (event->orientation() == Qt::Vertical)
    {
        cameraStrafeForward(&_current_camera, (double)event->delta() * factor);
        updateGL();

    }
    event->accept();
}

void WidgetWanderer::initializeGL()
{
    glClearColor(0.4, 0.7, 0.8, 0.0);

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE);

    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glDepthFunc(GL_LESS);
    glDepthMask(true);
    glEnable(GL_DEPTH_TEST);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.0);
}

void WidgetWanderer::resizeGL(int w, int h)
{
    cameraSetRenderSize(&_current_camera, w, h);
    
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(_current_camera.yfov * 180.0 / M_PI, _current_camera.xratio, _current_camera.znear, _current_camera.zfar);
}

static inline void _renderTerrainQuad(TerrainDefinition* terrain, double x, double z, double size)
{
    double y1, y2, y3, y4;

    y1 = terrainGetHeight(terrain, x, z);
    y2 = terrainGetHeight(terrain, x, z + size);
    y3 = terrainGetHeight(terrain, x + size, z + size);
    y4 = terrainGetHeight(terrain, x + size, z);

    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINE_LOOP);
    glVertex3f(x, y1, z);
    glVertex3f(x, y2, z + size);
    glVertex3f(x + size, y3, z + size);
    glVertex3f(x + size, y4, z);
    glEnd();

    glColor3f(0.0, 0.5, 0.0);
    glBegin(GL_QUADS);
    glVertex3f(x, y1, z);
    glVertex3f(x, y2, z + size);
    glVertex3f(x + size, y3, z + size);
    glVertex3f(x + size, y4, z);
    glEnd();
}

static void _renderTerrain(TerrainDefinition* terrain, CameraDefinition* camera, int quality)
{
    int chunk_factor, chunk_count, i;
    double min_chunk_size, visible_chunk_size;
    double radius_int, radius_ext, chunk_size;
    double cx, cz;

    min_chunk_size = 1.0 / (double)quality;
    visible_chunk_size = 1.0 / (double)quality;

    cx = camera->location.x - fmod(camera->location.x, min_chunk_size * 16.0);
    cz = camera->location.z - fmod(camera->location.z, min_chunk_size * 16.0);

    chunk_factor = 1;
    chunk_count = 2;
    radius_int = 0.0;
    radius_ext = min_chunk_size;
    chunk_size = min_chunk_size;

    while (radius_ext < 100.0)
    {
        for (i = 0; i < chunk_count - 1; i++)
        {
            _renderTerrainQuad(terrain, cx - radius_ext + chunk_size * i, cz - radius_ext, chunk_size);
            _renderTerrainQuad(terrain, cx + radius_int, cz - radius_ext + chunk_size * i, chunk_size);
            _renderTerrainQuad(terrain, cx + radius_int - chunk_size * i, cz + radius_int, chunk_size);
            _renderTerrainQuad(terrain, cx - radius_ext, cz + radius_int - chunk_size * i, chunk_size);
        }

        if (chunk_count % 32 == 0 && chunk_size / radius_int < visible_chunk_size)
        {
            chunk_count /= 2;
            chunk_factor *= 2;
            /* TODO Fill in gaps with triangles */
        }
        chunk_count += 2;
        chunk_size = min_chunk_size * chunk_factor;
        radius_int = radius_ext;
        radius_ext += chunk_size;
    }
}

void WidgetWanderer::paintGL()
{
    QTime start_time;
    double frame_time;
    
    cameraValidateDefinition(&_current_camera, 1);
    
    start_time = QTime::currentTime();
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(_current_camera.location.x, _current_camera.location.y, _current_camera.location.z, _current_camera.target.x, _current_camera.target.y, _current_camera.target.z, _current_camera.up.x, _current_camera.up.y, _current_camera.up.z);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_QUADS);
    glVertex3f(-500.0, water.height, -500.0);
    glVertex3f(-500.0, water.height, 500.0);
    glVertex3f(500.0, water.height, 500.0);
    glVertex3f(500.0, water.height, -500.0);
    glEnd();

    _renderTerrain(&terrain, &_current_camera, quality);
    
    frame_time = 0.001 * (double)start_time.msecsTo(QTime::currentTime());
    
    average_frame_time = average_frame_time * 0.8 + frame_time * 0.2;
    //printf("%d %f\n", quality, average_frame_time);
    
    if (average_frame_time > 0.1 && quality > 1)
    {
        quality--;
    }
    if (average_frame_time < 0.04 && quality < 10)
    {
        quality++;
    }
}
