#include "widgetheightmap.h"

#include <QTime>
#include <math.h>
#include <GL/glu.h>
#include "tools.h"
#include "../lib_paysages/terrain.h"
#include "../lib_paysages/scenery.h"

WidgetHeightMap::WidgetHeightMap(QWidget *parent, HeightMap* heightmap):
    QGLWidget(parent)
{
    setMinimumSize(500, 500);
    setFocusPolicy(Qt::StrongFocus);

    _heightmap = heightmap;
    
    _average_frame_time = 0.0;
    
    _last_mouse_x = 0;
    _last_mouse_y = 0;
    
    _angle_h = 0.0;
    _angle_v = 0.3;
}

WidgetHeightMap::~WidgetHeightMap()
{
}

void WidgetHeightMap::setHorizontalViewAngle(double angle_h)
{
    _angle_h = angle_h;
    updateGL();
}

void WidgetHeightMap::setVerticalViewAngle(double angle_v)
{
    _angle_v = angle_v;
    updateGL();
}

void WidgetHeightMap::resetToTerrain()
{
    TerrainDefinition terrain;
    
    terrain = terrainCreateDefinition();
    sceneryGetTerrain(&terrain);
    
    // TODO Apply geoarea
    int rx = _heightmap->resolution_x;
    int rz = _heightmap->resolution_z;
    for (int x = 0; x < rx; x++)
    {
        for (int z = 0; z < rz; z++)
        {
            _heightmap->data[z * rx + x] = terrainGetHeight(&terrain, 80.0 * (double)x / (double)(rx - 1) - 40.0, 80.0 * (double)z / (double)(rz - 1) - 40.0);
        }
    }

    terrainDeleteDefinition(&terrain);
    updateGL();
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
    gluPerspective(1.57 * 180.0 / M_PI, 1.0, 1.0, 1000.0);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void WidgetHeightMap::paintGL()
{
    GLenum error_code;
    QTime start_time;
    double frame_time;
    int rx, rz;
    
    start_time = QTime::currentTime();
    
    // Place camera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(50.0 * cos(_angle_h) * cos(_angle_v), 50.0 * sin(_angle_v), -50.0 * sin(_angle_h) * cos(_angle_v), 0.0, 0.0, 0.0, -cos(_angle_h) * sin(_angle_v), cos(_angle_v), sin(_angle_h) * sin(_angle_v));

    // Background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glBegin(GL_POINTS);
    glColor3d(1.0, 0.0, 0.0);
    rx = _heightmap->resolution_x;
    rz = _heightmap->resolution_z;
    for (int x = 0; x < rx; x++)
    {
        for (int z = 0; z < rz; z++)
        {
            glVertex3d(80.0 * (double)x / (double)(rx - 1) - 40.0, _heightmap->data[z * rx + x], 80.0 * (double)z / (double)(rz - 1) - 40.0);
        }
    }
    glEnd();

    // Time stats
    frame_time = 0.001 * (double)start_time.msecsTo(QTime::currentTime());
    _average_frame_time = _average_frame_time * 0.8 + frame_time * 0.2;
    //printf("%d %f\n", quality, average_frame_time);
    
    while ((error_code = glGetError()) != GL_NO_ERROR)
    {
        logDebug(QString("[OpenGL] ERROR : ") + (const char*)gluErrorString(error_code));
    }
}
