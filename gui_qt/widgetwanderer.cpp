#include "widgetwanderer.h"

#include <QGLWidget>
#include <QKeyEvent>
#include <QTime>
#include <math.h>
#include <GL/glu.h>
#include <QThread>
#include "../lib_paysages/scenery.h"
#include "../lib_paysages/euclid.h"
#include "explorerchunkterrain.h"

class ChunkMaintenanceThread:public QThread
{
public:
    ChunkMaintenanceThread(WidgetWanderer* wanderer)
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
    WidgetWanderer* _wanderer;
};

static QVector<ChunkMaintenanceThread*> _threads;

static double _getTerrainHeight(Renderer* renderer, double x, double z)
{
    return terrainGetHeight((TerrainDefinition*)(renderer->customData[0]), x, z);
}

static Color _applyTextures(Renderer* renderer, Vector3 location, double precision)
{
    return texturesGetColor((TexturesDefinition*)(renderer->customData[1]), renderer, location.x, location.z, precision);
}

static void _alterLight(Renderer* renderer, LightDefinition* light, Vector3 location)
{
    light->color = terrainLightFilter((TerrainDefinition*)(renderer->customData[0]), renderer, light->color, location, v3Scale(light->direction, -1000.0), v3Scale(light->direction, -1.0));
}

static void _getLightStatus(Renderer* renderer, LightStatus* status, Vector3 location)
{
    lightingGetStatus((LightingDefinition*)renderer->customData[2], renderer, location, status);
}

WidgetWanderer::WidgetWanderer(QWidget *parent, CameraDefinition* camera):
    QGLWidget(parent)
{
    setMinimumSize(400, 300);
    setFocusPolicy(Qt::StrongFocus);

    _base_camera = camera;
    cameraCopyDefinition(camera, &_current_camera);

    _water = waterCreateDefinition();
    sceneryGetWater(&_water);
    _sky = skyCreateDefinition();
    sceneryGetSky(&_sky);
    _terrain = terrainCreateDefinition();
    sceneryGetTerrain(&_terrain);
    _textures = texturesCreateDefinition();
    sceneryGetTextures(&_textures);
    _lighting = lightingCreateDefinition();
    sceneryGetLighting(&_lighting);
    
    _renderer = rendererCreate();
    _renderer.render_quality = 3;
    _renderer.customData[0] = &_terrain;
    _renderer.customData[1] = &_textures;
    _renderer.customData[2] = &_lighting;
    _renderer.customData[3] = &_water;
    _renderer.applyTextures = _applyTextures;
    _renderer.getTerrainHeight = _getTerrainHeight;
    _renderer.alterLight = _alterLight;
    _renderer.getLightStatus = _getLightStatus;
    
    _updated = false;

    int chunks = 20;
    double size = 200.0;
    double chunksize = size / (double)chunks;
    double start = -size / 2.0;
    for (int i = 0; i < chunks; i++)
    {
        for (int j = 0; j < chunks; j++)
        {
            ExplorerChunkTerrain* chunk = new ExplorerChunkTerrain(&_renderer, start + chunksize * (double)i, start + chunksize * (double)j, chunksize, chunks);
            _chunks.append(chunk);
            _updateQueue.append(chunk);
        }
    }

    startThreads();
    startTimer(500);
    
    _average_frame_time = 0.05;
    _quality = 3;
    _last_mouse_x = 0;
    _last_mouse_y = 0;
}

WidgetWanderer::~WidgetWanderer()
{
    stopThreads();
    
    for (int i = 0; i < _chunks.count(); i++)
    {
        delete _chunks[i];
    }
    waterDeleteDefinition(&_water);
}

void WidgetWanderer::startThreads()
{
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

void WidgetWanderer::stopThreads()
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

void WidgetWanderer::performChunksMaintenance()
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

void WidgetWanderer::timerEvent(QTimerEvent *event)
{
    if (_updated)
    {
        _updated = false;
        updateGL();
    }
    
    for (int i = 0; i < _chunks.count(); i++)
    {
        _chunks[i]->updatePriority(&_current_camera);
    }
    _lock_chunks.lock();
    qSort(_updateQueue.begin(), _updateQueue.end(), _cmpChunks);
    _lock_chunks.unlock();
}

void WidgetWanderer::initializeGL()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);

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
    
    if (_current_camera.location.y > 30.0)
    {
        _current_camera.location.y = 30.0;
    }
    
    cameraValidateDefinition(&_current_camera, 1);
    
    start_time = QTime::currentTime();
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(_current_camera.location.x, _current_camera.location.y, _current_camera.location.z, _current_camera.target.x, _current_camera.target.y, _current_camera.target.z, _current_camera.up.x, _current_camera.up.y, _current_camera.up.z);

    // Background
    Color zenith_color = skyGetZenithColor(&_sky);
    glClearColor(zenith_color.r, zenith_color.g, zenith_color.b, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render sun
    Vector3 sun_location = v3Add(_current_camera.location, v3Scale(skyGetSunDirection(&_sky), 500.0));
    Color sun_color = skyGetSunColor(&_sky);
    glDisable(GL_TEXTURE);
    glDisable(GL_TEXTURE_2D);
    glColor3f(sun_color.r, sun_color.g, sun_color.b);
    glPointSize(15.0 * _sky.sun_radius / 0.02);
    glEnable(GL_POINT_SMOOTH);
    glBegin(GL_POINTS);
    glVertex3f(sun_location.x, sun_location.y, sun_location.z);
    glEnd();

    // Render water
    glDisable(GL_TEXTURE);
    glDisable(GL_TEXTURE_2D);
    glColor3f(_water.material.base.r, _water.material.base.g, _water.material.base.b);
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
        glColor3f(1.0, 1.0, 1.0);
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
