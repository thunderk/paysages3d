#include "widgetheightmap.h"

#include <QTime>
#include <math.h>
#include <GL/glu.h>
#include "tools.h"

WidgetHeightMap::WidgetHeightMap(QWidget *parent, HeightMap* heightmap):
    QGLWidget(parent)
{
    setMinimumSize(500, 500);
    setFocusPolicy(Qt::StrongFocus);

    _average_frame_time = 0.0;
    _last_mouse_x = 0;
    _last_mouse_y = 0;
}

WidgetHeightMap::~WidgetHeightMap()
{
}

void WidgetHeightMap::keyPressEvent(QKeyEvent* event)
{
}

void WidgetHeightMap::mousePressEvent(QMouseEvent* event)
{
}

void WidgetHeightMap::mouseMoveEvent(QMouseEvent* event)
{
}

void WidgetHeightMap::wheelEvent(QWheelEvent* event)
{
}

void WidgetHeightMap::initializeGL()
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

void WidgetHeightMap::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluPerspective(_current_camera.yfov * 180.0 / M_PI, _current_camera.xratio, _current_camera.znear, _current_camera.zfar);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void WidgetHeightMap::paintGL()
{
    GLenum error_code;
    QTime start_time;
    double frame_time;
    
    start_time = QTime::currentTime();
    
    // Place camera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //gluLookAt(_current_camera.location.x, _current_camera.location.y, _current_camera.location.z, _current_camera.target.x, _current_camera.target.y, _current_camera.target.z, _current_camera.up.x, _current_camera.up.y, _current_camera.up.z);

    // Background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Time stats
    frame_time = 0.001 * (double)start_time.msecsTo(QTime::currentTime());
    _average_frame_time = _average_frame_time * 0.8 + frame_time * 0.2;
    //printf("%d %f\n", quality, average_frame_time);
    
    while ((error_code = glGetError()) != GL_NO_ERROR)
    {
        logDebug(QString("[OpenGL] ERROR : ") + (const char*)gluErrorString(error_code));
    }
}
