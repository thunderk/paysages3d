#include "WidgetExplorer.h"

#include <QGLWidget>
#include <QKeyEvent>
#include <QTime>
#include <cmath>
#include <GL/glu.h>
#include <QThread>
#include "SoftwareRenderer.h"
#include "OpenGLRenderer.h"
#include "WaterDefinition.h"
#include "SurfaceMaterial.h"
#include "CameraDefinition.h"
#include "ExplorerChunkTerrain.h"
#include "TerrainRenderer.h"
#include "WaterRenderer.h"
#include "Scenery.h"
#include "LightingManager.h"

WidgetExplorer::WidgetExplorer(QWidget *parent, CameraDefinition* camera, Scenery* scenery) :
QGLWidget(parent)
{
    setMinimumSize(400, 300);
    setFocusPolicy(Qt::StrongFocus);

    _current_camera = new CameraDefinition;
    _base_camera = camera;
    camera->copy(_current_camera);

    _renderer = new OpenGLRenderer(scenery);

    _average_frame_time = 0.05;
    _quality = 3;
    _last_mouse_x = 0;
    _last_mouse_y = 0;

    startTimer(500);
}

WidgetExplorer::~WidgetExplorer()
{
    delete _current_camera;
    delete _renderer;
}

void WidgetExplorer::setCameraFov(double fov)
{
    _current_camera->setFov(fov);
    updateGL();
}

void WidgetExplorer::resetCamera()
{
    _base_camera->copy(_current_camera);
    updateGL();
}

void WidgetExplorer::validateCamera()
{
    _current_camera->copy(_base_camera);
}

void WidgetExplorer::keyPressEvent(QKeyEvent* event)
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
        _current_camera->strafeForward(0.1 * factor);
        updateGL();
    }
    else if (event->key() == Qt::Key_Down)
    {
        _current_camera->strafeForward(-0.1 * factor);
        updateGL();
    }
    else if (event->key() == Qt::Key_Right)
    {
        _current_camera->strafeRight(0.1 * factor);
        updateGL();
    }
    else if (event->key() == Qt::Key_Left)
    {
        _current_camera->strafeRight(-0.1 * factor);
        updateGL();
    }
    else if (event->key() == Qt::Key_PageUp)
    {
        _current_camera->strafeUp(0.1 * factor);
        updateGL();
    }
    else if (event->key() == Qt::Key_PageDown)
    {
        _current_camera->strafeUp(-0.1 * factor);
        updateGL();
    }
    else
    {
        QGLWidget::keyPressEvent(event);
    }
}

void WidgetExplorer::mousePressEvent(QMouseEvent* event)
{
    _last_mouse_x = event->x();
    _last_mouse_y = event->y();

    event->ignore();
}

void WidgetExplorer::mouseMoveEvent(QMouseEvent* event)
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
        _current_camera->rotateYaw((double) (event->x() - _last_mouse_x) * factor * 0.1);
        _current_camera->rotatePitch((double) (event->y() - _last_mouse_y) * factor * 0.1);
        updateGL();
        event->accept();
    }
    else if (event->buttons() & Qt::RightButton)
    {
        _current_camera->strafeRight((double) (_last_mouse_x - event->x()) * factor);
        _current_camera->strafeUp((double) (event->y() - _last_mouse_y) * factor);
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

void WidgetExplorer::wheelEvent(QWheelEvent* event)
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
        _current_camera->strafeForward((double) event->delta() * factor);
        updateGL();
    }
    event->accept();
}

void WidgetExplorer::timerEvent(QTimerEvent*)
{
    updateGL();
}

void WidgetExplorer::initializeGL()
{
    _renderer->initialize();
}

void WidgetExplorer::resizeGL(int w, int h)
{
    _current_camera->setRenderSize(w, h);
    _renderer->resize(w, h);
}

void WidgetExplorer::paintGL()
{
    QTime start_time;
    double frame_time;

    // Don't do this at each frame, only on camera change
    _renderer->getScenery()->setCamera(_current_camera);
    _renderer->getScenery()->getCamera(_current_camera);
    _renderer->cameraChangeEvent(_current_camera);

    start_time = QTime::currentTime();

    // Background
    _renderer->paint();

    frame_time = 0.001 * (double) start_time.msecsTo(QTime::currentTime());

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

    // Messages
    /*if (!_inited)
    {
        glColor3f(0.0, 0.0, 0.0);
        renderText(6, height() - 10, tr("Please wait while loading scene..."));
        glColor3f(1.0, 1.0, 1.0);
        renderText(5, height() - 9, tr("Please wait while loading scene..."));
    }*/
}
