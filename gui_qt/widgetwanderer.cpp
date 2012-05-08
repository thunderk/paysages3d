#include "widgetwanderer.h"

#include <QGLWidget>
#include <QKeyEvent>
#include <QTime>
#include <math.h>
#include <GL/glu.h>
#include "../lib_paysages/scenery.h"

WidgetWanderer::WidgetWanderer(QWidget *parent, CameraDefinition* camera):
    QGLWidget(parent)
{
    setMinimumSize(400, 300);
    setFocusPolicy(Qt::StrongFocus);

    _base_camera = camera;
    cameraCopyDefinition(camera, &_current_camera);

    _water = waterCreateDefinition();
    sceneryGetWater(&_water);
    
    _renderer = sceneryCreateStandardRenderer();

    int chunks = 16;
    double size = 30.0;
    double chunksize = size / (double)chunks;
    double start = -size / 2.0;
    for (int i = 0; i < chunks; i++)
    {
        for (int j = 0; j < chunks; j++)
        {
            _chunks.append(new WandererChunk(start + chunksize * (double)i, start + chunksize * (double)j, chunksize));
        }
    }
    
    _average_frame_time = 0.05;
    _quality = 3;
    _last_mouse_x = 0;
    _last_mouse_y = 0;
}

WidgetWanderer::~WidgetWanderer()
{
    for (int i = 0; i < _chunks.count(); i++)
    {
        delete _chunks[i];
    }
    waterDeleteDefinition(&_water);
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

    if (event->modifiers() & Qt::ControlModifier)
    {
        factor = 0.1;
    }
    else if (event->modifiers() & Qt::ShiftModifier)
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
    _last_mouse_x = event->x();
    _last_mouse_y = event->y();

    event->ignore();
}

void WidgetWanderer::mouseMoveEvent(QMouseEvent* event)
{
    double factor;

    if (event->modifiers() & Qt::ControlModifier)
    {
        factor = 0.01;
    }
    else if (event->modifiers() & Qt::ShiftModifier)
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
        cameraRotateYaw(&_current_camera, (double)(event->x() - _last_mouse_x) * factor * 0.1);
        cameraRotatePitch(&_current_camera, (double)(event->y() - _last_mouse_y) * factor * 0.1);
        updateGL();
        event->accept();
    }
    else if (event->buttons() & Qt::RightButton)
    {
        cameraStrafeRight(&_current_camera, (double)(_last_mouse_x - event->x()) * factor);
        cameraStrafeUp(&_current_camera, (double)(event->y() - _last_mouse_y) * factor);
        updateGL();
        event->accept();
    }
    else
    {
        event->ignore();
    }

    _last_mouse_x = event->x();
    _last_mouse_y = event->y();
}

void WidgetWanderer::wheelEvent(QWheelEvent* event)
{
    double factor;

    if (event->modifiers() & Qt::ControlModifier)
    {
        factor = 0.01;
    }
    else if (event->modifiers() & Qt::ShiftModifier)
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

    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glDepthFunc(GL_LESS);
    glDepthMask(true);
    glEnable(GL_DEPTH_TEST);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.0);

    glDisable(GL_FOG);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void WidgetWanderer::resizeGL(int w, int h)
{
    cameraSetRenderSize(&_current_camera, w, h);
    
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(_current_camera.yfov * 180.0 / M_PI, _current_camera.xratio, _current_camera.znear, _current_camera.zfar);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

    // Render water
    glDisable(GL_TEXTURE);
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_QUADS);
    glVertex3f(_current_camera.location.x - 500.0, _water.height, _current_camera.location.z - 500.0);
    glVertex3f(_current_camera.location.x - 500.0, _water.height, _current_camera.location.z + 500.0);
    glVertex3f(_current_camera.location.x + 500.0, _water.height, _current_camera.location.z + 500.0);
    glVertex3f(_current_camera.location.x + 500.0, _water.height, _current_camera.location.z - 500.0);
    glEnd();

    // Render terrain chunks
    glEnable(GL_TEXTURE);
    glEnable(GL_TEXTURE_2D);
    for (int i = 0; i < _chunks.count(); i++)
    {
        _chunks[i]->maintain(&_renderer);
        
        _chunks[i]->render(this);
    }
    
    frame_time = 0.001 * (double)start_time.msecsTo(QTime::currentTime());
    
    _average_frame_time = _average_frame_time * 0.8 + frame_time * 0.2;
    //printf("%d %f\n", quality, average_frame_time);
    
    if (_average_frame_time > 0.1 && _quality > 1)
    {
        _quality--;
    }
    if (_average_frame_time < 0.04 && _quality < 10)
    {
        _quality++;
    }
}
