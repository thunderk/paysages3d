#include "widgetheightmap.h"

#include <QTime>
#include <QMouseEvent>
#include <QWidget>
#include <math.h>
#include <GL/glu.h>
#include "tools.h"
#include "rendering/scenery.h"

#define HEIGHTMAP_RESOLUTION 256

WidgetHeightMap::WidgetHeightMap(QWidget *parent, TerrainDefinition* terrain) :
QGLWidget(parent)
{
    setMinimumSize(500, 500);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setCursor(Qt::CrossCursor);
    startTimer(100);

    _memory_stats = terrainGetMemoryStats(terrain);

    _terrain = terrain;
    _renderer = rendererCreate();
    TerrainRendererClass.bind(_renderer, _terrain);
    _vertices = new _VertexInfo[HEIGHTMAP_RESOLUTION * HEIGHTMAP_RESOLUTION];

    _dirty = true;

    _water = true;
    _wireframe = true;
    WaterDefinition* water_definition = (WaterDefinition*) WaterDefinitionClass.create();
    sceneryGetWater(water_definition);
    _water_height = water_definition->height;
    WaterDefinitionClass.destroy(water_definition);

    _average_frame_time = 0.0;

    _last_brush_action = 0;
    _last_mouse_x = 0;
    _last_mouse_y = 0;
    _last_time = QDateTime::currentDateTime();
    _mouse_moved = false;

    _current_camera = cameraCreateDefinition();
    _top_camera = cameraCreateDefinition();
    _temp_camera = cameraCreateDefinition();
    _zoom = 50.0;

    Vector3 camera_location = {0.0, 80.0, 10.0};
    cameraSetLocation(_current_camera, camera_location);
    cameraSetTarget(_current_camera, VECTOR_ZERO);
    cameraSetZoomToTarget(_top_camera, _zoom);
    cameraCopyDefinition(_current_camera, _top_camera);

    _brush_x = 0.0;
    _brush_z = 0.0;
    _brush_mode = HEIGHTMAP_BRUSH_RAISE;
    _brush_size = 10.0;
    _brush_smoothing = 0.5;
    _brush_strength = 1.0;
    _brush_noise = noiseCreateGenerator();
    noiseAddLevelsSimple(_brush_noise, 10, 1.0, -0.5, 0.5, 0.5);
}

WidgetHeightMap::~WidgetHeightMap()
{
    cameraDeleteDefinition(_current_camera);
    cameraDeleteDefinition(_top_camera);
    cameraDeleteDefinition(_temp_camera);
    rendererDelete(_renderer);
    noiseDeleteGenerator(_brush_noise);
    delete[] _vertices;
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

QString WidgetHeightMap::getMemoryStats()
{
    qint64 memused = _memory_stats;
    if (memused >= 1024)
    {
        memused /= 1024;
        if (memused >= 1024)
        {
            memused /= 1024;
            if (memused >= 1024)
            {
                memused /= 1024;
                return tr("%1 GB").arg(memused);
            }
            else
            {
                return tr("%1 MB").arg(memused);
            }
        }
        else
        {
            return tr("%1 kB").arg(memused);
        }
    }
    else
    {
        return tr("%1 B").arg(memused);
    }
}

void WidgetHeightMap::revert()
{
    _dirty = true;
    updateGL();
}

void WidgetHeightMap::wheelEvent(QWheelEvent* event)
{
    if (event->orientation() == Qt::Vertical)
    {
        _zoom -= 0.05 * (double) event->delta();
        if (_zoom < 10.0)
        {
            _zoom = 10.0;
        }
        else if (_zoom > 70.0)
        {
            _zoom = 70.0;
        }

        cameraSetZoomToTarget(_top_camera, _zoom);
    }
    event->accept();
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

void WidgetHeightMap::mouseReleaseEvent(QMouseEvent*)
{
    _last_brush_action = 0;
    terrainEndBrushStroke(_terrain->height_map);
}

void WidgetHeightMap::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::MiddleButton)
    {
        // Rotate around the turntable
        int move_x = event->x() - _last_mouse_x;
        int move_y = event->y() - _last_mouse_y;

        // TODO
        /*_angle_h -= (double)move_x * 0.008;
        _angle_v += (double)move_y * 0.003;*/
    }

    _last_mouse_x = event->x();
    _last_mouse_y = event->y();
    _mouse_moved = true;

    updateGL();
}

void WidgetHeightMap::timerEvent(QTimerEvent*)
{
    QDateTime new_time = QDateTime::currentDateTime();
    double duration = 0.001 * (double) _last_time.msecsTo(new_time);
    _last_time = new_time;

    if (not underMouse())
    {
        return;
    }

    if (_last_brush_action != 0)
    {
        double brush_strength;
        TerrainBrush brush;

        Vector3 camera_target = cameraGetTarget(_current_camera);
        double tx = camera_target.x;
        double tz = camera_target.z;

        brush.relative_x = _brush_x + tx;
        brush.relative_z = _brush_z + tz;
        brush.hard_radius = _brush_size * (1.0 - _brush_smoothing);
        brush.smoothed_size = _brush_size * _brush_smoothing;
        brush.total_radius = brush.hard_radius + brush.smoothed_size;

        brush_strength = _brush_strength * duration / 0.1;

        switch (_brush_mode)
        {
        case HEIGHTMAP_BRUSH_RAISE:
            terrainBrushElevation(_terrain->height_map, &brush, brush_strength * _last_brush_action * 20.0);
            break;
        case HEIGHTMAP_BRUSH_SMOOTH:
            if (_last_brush_action < 0)
            {
                terrainBrushSmooth(_terrain->height_map, &brush, brush_strength * 0.1);
            }
            else
            {
                terrainBrushAddNoise(_terrain->height_map, &brush, _brush_noise, brush_strength * 10.0);
            }
            break;
        case HEIGHTMAP_BRUSH_RESTORE:
            terrainBrushReset(_terrain->height_map, &brush, brush_strength);
            break;
        default:
            return;
        }

        // TODO Only mark dirty the updated area
        _dirty = true;
        updateGL();
    }

    // Move camera
    if (cameraTransitionToAnother(_current_camera, _top_camera, 0.8))
    {
        updateGL();
    }

    // TODO Apply scrolling to vertex info and dirty only needed area
    /*double edge_length = 10.0;
    if (_brush_x > HEIGHTMAP_RESOLUTION / 2.0 - edge_length)
    {
        double dx = HEIGHTMAP_RESOLUTION / 2.0 - edge_length - _brush_x;
        _position_x -= (int)ceil(dx);

        _dirty = true;
        updateGL();
    }
    if (_brush_x < -HEIGHTMAP_RESOLUTION / 2.0 + edge_length)
    {
        double dx = -HEIGHTMAP_RESOLUTION / 2.0 + edge_length - _brush_x;
        _position_x -= (int)ceil(dx);

        _dirty = true;
        updateGL();
    }
    if (_brush_z > HEIGHTMAP_RESOLUTION / 2.0 - edge_length)
    {
        double dz = HEIGHTMAP_RESOLUTION / 2.0 - edge_length - _brush_z;
        _position_z -= (int)ceil(dz);

        _dirty = true;
        updateGL();
    }
    if (_brush_z < -HEIGHTMAP_RESOLUTION / 2.0 + edge_length)
    {
        double dz = -HEIGHTMAP_RESOLUTION / 2.0 + edge_length - _brush_z;
        _position_z -= (int)ceil(dz);

        _dirty = true;
        updateGL();
    }*/
}

void WidgetHeightMap::initializeGL()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);

    GLfloat light_diffuse[] = {0.75, 0.74, 0.7, 1.0};
    GLfloat light_specular[] = {0.0, 0.0, 0.0, 0.0};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    light_diffuse[0] = 0.3;
    light_diffuse[1] = 0.3;
    light_diffuse[2] = 0.4;
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_COLOR_MATERIAL);
    //glColorMaterial(GL_FRONT_AND_BACK, GL_EMISSION);

    //glFrontFace(GL_CCW);
    //glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);
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

    rx = HEIGHTMAP_RESOLUTION;
    rz = HEIGHTMAP_RESOLUTION;

    // Update vertex cache
    if (_dirty)
    {
        updateVertexInfo();
        emit heightmapChanged();
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

        winX = (float) _last_mouse_x;
        winY = (float) height() - (float) _last_mouse_y;
        glReadPixels(_last_mouse_x, (int) winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

        if (winZ > 0.0 && winZ < 1000.0)
        {
            gluUnProject(winX, winY, winZ, modelview, projection, viewport, &point.x, &point.y, &point.z);

            if (point.x >= -rx / 2 && point.x <= rx / 2 && point.z >= -rz / 2 && point.z <= rz / 2)
            {
                _brush_x = point.x;
                _brush_z = point.z;
            }
        }

        _mouse_moved = false;
    }

    // Place camera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Vector3 camera_location = cameraGetLocation(_current_camera);
    Vector3 camera_target = cameraGetTarget(_current_camera);
    Vector3 camera_up = cameraGetUpVector(_current_camera);
    gluLookAt(camera_location.x, camera_location.y, camera_location.z, camera_target.x, camera_target.y, camera_target.z, camera_up.x, camera_up.y, camera_up.z);

    // Place lights
    GLfloat light_position[] = {40.0, 40.0, 40.0, 0.0};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    light_position[0] = -40.0;
    light_position[2] = -60.0;
    glLightfv(GL_LIGHT1, GL_POSITION, light_position);

    // Background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Height map
    camera_target = cameraGetTarget(_current_camera);
    double tx = camera_target.x;
    double tz = camera_target.z;
    for (int x = 0; x < rx - 1; x++)
    {
        glBegin(GL_QUAD_STRIP);
        for (int z = 0; z < rz; z++)
        {
            for (int dx = 1; dx >= 0; dx--)
            {
                _VertexInfo* vertex = _vertices + z * rx + x + dx;
                double diff_x, diff_z, diff;

                diff_x = vertex->point.x - tx - _brush_x;
                diff_z = vertex->point.z - tz - _brush_z;
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
                glColor3f(0.8 + diff, vertex->painted ? 1.0 : 0.8, 0.8);
                glNormal3f(vertex->normal.x, vertex->normal.y, vertex->normal.z);
                glVertex3f(vertex->point.x - tx, vertex->point.y, vertex->point.z - tz);
            }
        }
        glEnd();
    }

    // Wireframe
    if (_wireframe)
    {
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.8, 0.0, 0.0, 0.1);
        for (int z = 0; z < rz; z++)
        {
            glBegin(GL_LINE_STRIP);
            for (int x = 0; x < rx; x++)
            {
                _VertexInfo* vertex = _vertices + z * rx + x;
                glVertex3f(vertex->point.x - tx, vertex->point.y, vertex->point.z - tz);
            }
            glEnd();
        }
        for (int x = 0; x < rx; x++)
        {
            glBegin(GL_LINE_STRIP);
            for (int z = 0; z < rz; z++)
            {
                _VertexInfo* vertex = _vertices + z * rx + x;
                glVertex3f(vertex->point.x - tx, vertex->point.y, vertex->point.z - tz);
            }
            glEnd();
        }
        glEnable(GL_LIGHTING);
        glDisable(GL_BLEND);
    }

    // Water
    if (_water)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.2, 0.3, 0.7, 0.7);
        glBegin(GL_QUADS);
        glVertex3f(camera_target.x - 500.0, _water_height, camera_target.z - 500.0);
        glVertex3f(camera_target.x - 500.0, _water_height, camera_target.z + 500.0);
        glVertex3f(camera_target.x + 500.0, _water_height, camera_target.z + 500.0);
        glVertex3f(camera_target.x + 500.0, _water_height, camera_target.z - 500.0);
        glEnd();
        glDisable(GL_BLEND);
    }

    // Time stats
    frame_time = 0.001 * (double) start_time.msecsTo(QTime::currentTime());
    _average_frame_time = _average_frame_time * 0.8 + frame_time * 0.2;
    //printf("%d %f\n", quality, average_frame_time);

    while ((error_code = glGetError()) != GL_NO_ERROR)
    {
        logDebug(QString("[OpenGL] ERROR : ") + (const char*) gluErrorString(error_code));
    }
}

void WidgetHeightMap::updateVertexInfo()
{
    int rx = HEIGHTMAP_RESOLUTION;
    int rz = HEIGHTMAP_RESOLUTION;
    int dx, dz;

    _VertexInfo* old_vertices = _vertices;
    _vertices = new _VertexInfo[rx * rz];
    delete[] old_vertices;

    _memory_stats = terrainGetMemoryStats(_terrain);

    // Update positions
    Vector3 camera_target = cameraGetTarget(_current_camera);
    double tx = camera_target.x;
    double tz = camera_target.z;
    for (int x = 0; x < rx; x++)
    {
        for (int z = 0; z < rz; z++)
        {
            _VertexInfo* vertex = _vertices + z * rx + x;

            dx = tx + x - rx / 2;
            dz = tz + z - rz / 2;

            vertex->point.x = (double) dx;
            vertex->point.z = (double) dz;

            vertex->point.y = terrainGetGridHeight(_terrain, dx, dz, 1);

            vertex->painted = terrainIsPainted(_terrain->height_map, dx, dz);
        }
    }

    // Update normals
    for (int x = 0; x < rx; x++)
    {
        for (int z = 0; z < rz; z++)
        {
            _VertexInfo* vertex = _vertices + z * rx + x;

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
