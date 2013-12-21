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

class ChunkMaintenanceThread : public QThread
{
public:

    ChunkMaintenanceThread(WidgetExplorer* wanderer)
    {
        _wanderer = wanderer;
        _running = true;
    }

    void askStop()
    {
        _running = false;
    }

    static inline void usleep(unsigned long us)
    {
        QThread::usleep(us);
    }

protected:

    void run()
    {
        while (_running)
        {
            _wanderer->performChunksMaintenance();
            QThread::usleep(10000);
        }
    }

private:
    bool _running;
    WidgetExplorer* _wanderer;
};

static QVector<ChunkMaintenanceThread*> _threads;

WidgetExplorer::WidgetExplorer(QWidget *parent, CameraDefinition* camera, Scenery* scenery) :
QGLWidget(parent)
{
    setMinimumSize(400, 300);
    setFocusPolicy(Qt::StrongFocus);

    _current_camera = new CameraDefinition;
    _base_camera = camera;
    camera->copy(_current_camera);

    _renderer = new OpenGLRenderer(scenery);
    _renderer->prepare();
    _renderer->render_quality = 3;
    _renderer->getLightingManager()->setSpecularity(false);
    _renderer->disableClouds();

    _inited = false;
    _updated = false;

    _average_frame_time = 0.05;
    _quality = 3;
    _last_mouse_x = 0;
    _last_mouse_y = 0;

    startTimer(500);
}

WidgetExplorer::~WidgetExplorer()
{
    stopRendering();

    for (int i = 0; i < _chunks.count(); i++)
    {
        delete _chunks[i];
    }
    delete _current_camera;
    delete _renderer;
}

void WidgetExplorer::startRendering()
{
    // Add terrain
    int chunks = 20;
    double size = 400.0;
    double chunksize = size / (double) chunks;
    double start = -size / 2.0;
    double water_height = _renderer->getWaterRenderer()->getHeightInfo().base_height;
    for (int i = 0; i < chunks; i++)
    {
        for (int j = 0; j < chunks; j++)
        {
            ExplorerChunkTerrain* chunk = new ExplorerChunkTerrain(_renderer, start + chunksize * (double) i, start + chunksize * (double) j, chunksize, chunks, water_height);
            _chunks.append(chunk);
            _updateQueue.append(chunk);
        }
    }

    // Start rendering workers
    int nbcore;
    _alive = true;

    nbcore = QThread::idealThreadCount();
    if (nbcore < 1)
    {
        nbcore = 1;
    }

    for (int i = 0; i < nbcore; i++)
    {
        _threads.append(new ChunkMaintenanceThread(this));
    }
    for (int i = 0; i < _threads.count(); i++)
    {
        _threads[i]->start();
    }
}

void WidgetExplorer::stopRendering()
{
    for (int i = 0; i < _threads.count(); i++)
    {
        _threads[i]->askStop();
    }
    _alive = false;
    for (int i = 0; i < _threads.count(); i++)
    {
        _threads[i]->wait();
    }
}

bool _cmpChunks(const BaseExplorerChunk* c1, const BaseExplorerChunk* c2)
{
    return c1->priority > c2->priority;
}

void WidgetExplorer::performChunksMaintenance()
{
    BaseExplorerChunk* chunk;

    _lock_chunks.lock();
    if (_updateQueue.count() > 0)
    {
        chunk = _updateQueue.takeFirst();
        _lock_chunks.unlock();
    }
    else
    {
        _lock_chunks.unlock();
        return;
    }

    if (chunk->maintain())
    {
        if (!_alive)
        {
            return;
        }

        _updated = true;
    }

    _lock_chunks.lock();
    _updateQueue.append(chunk);
    _lock_chunks.unlock();
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
    if (!_inited)
    {
        _inited = true;
        startRendering();
    }

    if (_updated)
    {
        _updated = false;
        updateGL();
    }

    for (int i = 0; i < _chunks.count(); i++)
    {
        _chunks[i]->updatePriority(_current_camera);
    }
    _lock_chunks.lock();
    qSort(_updateQueue.begin(), _updateQueue.end(), _cmpChunks);
    _lock_chunks.unlock();
}

void WidgetExplorer::initializeGL()
{
    _renderer->initialize();
}

void WidgetExplorer::resizeGL(int w, int h)
{
    _current_camera->setRenderSize(w, h);
    _renderer->resize(w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    CameraPerspective perspective = _current_camera->getPerspective();
    gluPerspective(perspective.yfov * 180.0 / M_PI, perspective.xratio, perspective.znear, perspective.zfar);
}

void WidgetExplorer::paintGL()
{
    GLenum error_code;
    QTime start_time;
    double frame_time;
    WaterDefinition* water = _renderer->getScenery()->getWater();

    // Don't do this at each frame, only on camera change
    _renderer->getScenery()->setCamera(_current_camera);
    _renderer->getScenery()->getCamera(_current_camera);
    _renderer->cameraChangeEvent(_current_camera);

    start_time = QTime::currentTime();

    // Background
    _renderer->paint();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Vector3 camera_location = _current_camera->getLocation();
    Vector3 camera_target = _current_camera->getTarget();
    Vector3 camera_up = _current_camera->getUpVector();
    gluLookAt(camera_location.x, camera_location.y, camera_location.z, camera_target.x, camera_target.y, camera_target.z, camera_up.x, camera_up.y, camera_up.z);

    // Render water
    double water_height = _renderer->getTerrainRenderer()->getWaterHeight();
    glDisable(GL_TEXTURE_2D);
    glColor3f(water->material->_rgb.r, water->material->_rgb.g, water->material->_rgb.b);
    glBegin(GL_QUADS);
    glVertex3f(camera_location.x - 500.0, water_height, camera_location.z - 500.0);
    glVertex3f(camera_location.x - 500.0, water_height, camera_location.z + 500.0);
    glVertex3f(camera_location.x + 500.0, water_height, camera_location.z + 500.0);
    glVertex3f(camera_location.x + 500.0, water_height, camera_location.z - 500.0);
    glEnd();

    // Render chunks
    glEnable(GL_TEXTURE_2D);
    for (int i = 0; i < _chunks.count(); i++)
    {
        glColor3f(1.0, 1.0, 1.0);
        _chunks[i]->render(this);
    }

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
    if (!_inited)
    {
        glColor3f(0.0, 0.0, 0.0);
        renderText(6, height() - 10, tr("Please wait while loading scene..."));
        glColor3f(1.0, 1.0, 1.0);
        renderText(5, height() - 9, tr("Please wait while loading scene..."));
    }

    while ((error_code = glGetError()) != GL_NO_ERROR)
    {
        qDebug("[OpenGL] ERROR : %s", (const char*)gluErrorString(error_code));
    }
}
