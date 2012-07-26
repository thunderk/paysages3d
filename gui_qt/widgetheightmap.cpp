#include "widgetheightmap.h"

#include <QTime>
#include <QMouseEvent>
#include <math.h>
#include <GL/glu.h>
#include <qt4/QtCore/qlocale.h>
#include "tools.h"
#include "../lib_paysages/terrain.h"
#include "../lib_paysages/scenery.h"

WidgetHeightMap::WidgetHeightMap(QWidget *parent, HeightMap* heightmap):
    QGLWidget(parent)
{
    setMinimumSize(500, 500);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setCursor(Qt::CrossCursor);
    startTimer(100);

    _heightmap = heightmap;
    _vertexes = new _VertexInfo[heightmap->resolution_x * heightmap->resolution_z];
    
    _dirty = true;
    
    _average_frame_time = 0.0;
    
    _last_brush_action = 0;
    _last_mouse_x = 0;
    _last_mouse_y = 0;
    _last_time = QDateTime::currentDateTime();
    _mouse_moved = false;
    
    _angle_h = 0.0;
    _angle_v = 0.3;
    
    _brush_x = 0.0;
    _brush_z = 0.0;
    _brush_mode = HEIGHTMAP_BRUSH_RAISE;
    _brush_size = 10.0;
    _brush_smoothing = 0.5;
    _brush_strength = 1.0;
    _brush_noise = noiseCreateGenerator();
    noiseGenerateBaseNoise(_brush_noise, 102400);
    noiseAddLevelsSimple(_brush_noise, 6, 1.0, 1.0);
}

WidgetHeightMap::~WidgetHeightMap()
{
    noiseDeleteGenerator(_brush_noise);
    delete[] _vertexes;
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

void WidgetHeightMap::setBrushMode(HeightMapBrushMode mode)
{
    _brush_mode = mode;
    _brush_x = 0.0;
    _brush_z = 0.0;
    updateGL();
}

void WidgetHeightMap::setBrushSize(double size)
{
    _brush_size = size;
    _brush_x = 0.0;
    _brush_z = 0.0;
    updateGL();
}

void WidgetHeightMap::setBrushSmoothing(double smoothing)
{
    _brush_smoothing = smoothing;
    _brush_x = 0.0;
    _brush_z = 0.0;
    updateGL();
}

void WidgetHeightMap::setBrushStrength(double strength)
{
    _brush_strength = strength;
    _brush_x = 0.0;
    _brush_z = 0.0;
    updateGL();
}

void WidgetHeightMap::revert()
{
    _dirty = true;
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
    revert();
}

void WidgetHeightMap::mousePressEvent(QMouseEvent* event)
{
    _last_mouse_x = event->x();
    _last_mouse_y = event->y();
    if (event->buttons() & Qt::LeftButton)
    {
        _last_brush_action = 1;
    }
    else if (event->buttons() & Qt::RightButton)
    {
        _last_brush_action = -1;
    }
}

void WidgetHeightMap::mouseReleaseEvent(QMouseEvent* event)
{
    _last_brush_action = 0;
}

void WidgetHeightMap::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::MiddleButton)
    {
        // Rotate around the turntable
        int move_x = event->x() - _last_mouse_x;
        int move_y = event->y() - _last_mouse_y;
        
        _angle_h -= (double)move_x * 0.008;
        _angle_v += (double)move_y * 0.003;
    }
    
    _last_mouse_x = event->x();
    _last_mouse_y = event->y();
    _mouse_moved = true;

    updateGL();
}

void WidgetHeightMap::timerEvent(QTimerEvent*)
{
    QDateTime new_time = QDateTime::currentDateTime();
    double duration = 0.001 * (double)_last_time.msecsTo(new_time);
    _last_time = new_time;
    
    if (_last_brush_action != 0)
    {
        double brush_strength;
        HeightMapBrush brush;

        brush.relative_x = (_brush_x + 40.0) / 80.0;
        brush.relative_z = (_brush_z + 40.0) / 80.0;
        brush.hard_radius = _brush_size * (1.0 - _brush_smoothing) / 80.0;
        brush.smoothed_size = _brush_size * _brush_smoothing / 80.0;
        brush.total_radius = brush.hard_radius + brush.smoothed_size;
        
        brush_strength = _brush_strength * duration / 0.1;
        
        switch (_brush_mode)
        {
            case HEIGHTMAP_BRUSH_RAISE:
                heightmapBrushElevation(_heightmap, &brush, brush_strength * _last_brush_action * 20.0);
                break;
            case HEIGHTMAP_BRUSH_SMOOTH:
                if (_last_brush_action < 0)
                {
                    heightmapBrushSmooth(_heightmap, &brush, brush_strength);
                }
                else
                {
                    heightmapBrushAddNoise(_heightmap, &brush, _brush_noise, brush_strength * 10.0);
                }
                break;
            default:
                return;
        }
        
        // TODO Only mark dirty the updated area
        _dirty = true;
        updateGL();
    }
}

void WidgetHeightMap::initializeGL()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);

    GLfloat light_position[] = { 40.0, 40.0, 40.0, 0.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 0.0, 0.0, 0.0, 0.0 };
    glShadeModel(GL_SMOOTH);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
   
    //glFrontFace(GL_CCW);
    //glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);

    glDepthFunc(GL_LESS);
    glDepthMask(true);
    glEnable(GL_DEPTH_TEST);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //glEnable(GL_LINE_SMOOTH);
    //glLineWidth(1.0);

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
    
    // Update vertex cache
    if (_dirty)
    {
        updateVertexInfo();
        _dirty = false;
    }
    
    // Picking mouse position using z-buffer (for brush)
    if (_mouse_moved)
    {
        GLint viewport[4];
        GLdouble modelview[16];
        GLdouble projection[16];
        GLfloat winX, winY, winZ;
        Vector3 point;

        glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
        glGetDoublev(GL_PROJECTION_MATRIX, projection);
        glGetIntegerv(GL_VIEWPORT, viewport);

        winX = (float)_last_mouse_x;
        winY = (float)height() - (float)_last_mouse_y;
        glReadPixels(_last_mouse_x, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

        gluUnProject(winX, winY, winZ, modelview, projection, viewport, &point.x, &point.y, &point.z);

        _brush_x = point.x;
        _brush_z = point.z;
        _mouse_moved = false;
    }
    
    // Place camera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(50.0 * cos(_angle_h) * cos(_angle_v), 50.0 * sin(_angle_v), -50.0 * sin(_angle_h) * cos(_angle_v), 0.0, 0.0, 0.0, -cos(_angle_h) * sin(_angle_v), cos(_angle_v), sin(_angle_h) * sin(_angle_v));

    // Background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Height map
    rx = _heightmap->resolution_x;
    rz = _heightmap->resolution_z;
    for (int x = 0; x < rx - 1; x++)
    {
        glBegin(GL_QUAD_STRIP);
        for (int z = 0; z < rz; z++)
        {
            _VertexInfo* vertex = _vertexes + z * rx + x;
            double diff_x, diff_z, diff;
            
            diff_x = (vertex + 1)->point.x - _brush_x;
            diff_z = (vertex + 1)->point.z - _brush_z;
            diff = sqrt(diff_x * diff_x + diff_z * diff_z);
            if (diff > _brush_size)
            {
                diff = 0.0;
            }
            else if (diff > _brush_size * (1.0 - _brush_smoothing))
            {
                diff = 1.0 - (diff - _brush_size * (1.0 - _brush_smoothing)) / (_brush_size * _brush_smoothing);
            }
            else
            {
                diff = 1.0;
            }
            glColor3d(1.0, 1.0 - diff, 1.0 - diff);
            glNormal3d((vertex + 1)->normal.x, (vertex + 1)->normal.y, (vertex + 1)->normal.z);
            glVertex3d((vertex + 1)->point.x, (vertex + 1)->point.y, (vertex + 1)->point.z);
            
            diff_x = vertex->point.x - _brush_x;
            diff_z = vertex->point.z - _brush_z;
            diff = sqrt(diff_x * diff_x + diff_z * diff_z);
            if (diff > _brush_size)
            {
                diff = 0.0;
            }
            else if (diff > _brush_size * (1.0 - _brush_smoothing))
            {
                diff = 1.0 - (diff - _brush_size * (1.0 - _brush_smoothing)) / (_brush_size * _brush_smoothing);
            }
            else
            {
                diff = 1.0;
            }
            glColor3d(1.0, 1.0 - diff, 1.0 - diff);
            glNormal3d(vertex->normal.x, vertex->normal.y, vertex->normal.z);
            glVertex3d(vertex->point.x, vertex->point.y, vertex->point.z);
        }
        glEnd();
    }

    // Time stats
    frame_time = 0.001 * (double)start_time.msecsTo(QTime::currentTime());
    _average_frame_time = _average_frame_time * 0.8 + frame_time * 0.2;
    //printf("%d %f\n", quality, average_frame_time);
    
    while ((error_code = glGetError()) != GL_NO_ERROR)
    {
        logDebug(QString("[OpenGL] ERROR : ") + (const char*)gluErrorString(error_code));
    }
}

void WidgetHeightMap::updateVertexInfo()
{
    int rx = _heightmap->resolution_x;
    int rz = _heightmap->resolution_z;
    
    // Update positions
    for (int x = 0; x < rx; x++)
    {
        for (int z = 0; z < rz; z++)
        {
            _VertexInfo* vertex = _vertexes + z * rx + x;
            
            vertex->point.x = 80.0 * (double)x / (double)(rx - 1) - 40.0;
            vertex->point.y = _heightmap->data[z * rx + x];
            vertex->point.z = 80.0 * (double)z / (double)(rz - 1) - 40.0;
        }
    }
    
    // Update normals
    for (int x = 0; x < rx; x++)
    {
        for (int z = 0; z < rz; z++)
        {
            _VertexInfo* vertex = _vertexes + z * rx + x;
            
            if (x == rx - 1)
            {
                vertex->normal = (vertex - 1)->normal;
            }
            else if (z == rz - 1)
            {
                vertex->normal = (vertex - rx)->normal;
            }
            else
            {
                Vector3 dx, dz;
                
                dx = v3Sub((vertex + 1)->point, vertex->point);
                dz = v3Sub((vertex + rx)->point, vertex->point);
                
                vertex->normal = v3Cross(v3Normalize(dz), v3Normalize(dx));
            }
        }
    }
}
