#ifndef _PAYSAGES_QT_WIDGETHEIGHTMAP_H_
#define _PAYSAGES_QT_WIDGETHEIGHTMAP_H_

#include <QGLWidget>
#include <QDateTime>
#include "rendering/camera.h"
#include "rendering/tools/euclid.h"
#include "rendering/renderer.h"
#include "rendering/terrain/public.h"

typedef struct
{
    Vector3 point;
    Vector3 normal;
    int painted;
} _VertexInfo;

typedef enum
{
    HEIGHTMAP_BRUSH_RAISE = 0,
    HEIGHTMAP_BRUSH_SMOOTH = 1,
    HEIGHTMAP_BRUSH_RESTORE = 2
} HeightMapBrushMode;

class WidgetHeightMap : public QGLWidget
{
    Q_OBJECT
public:
    WidgetHeightMap(QWidget* parent, TerrainDefinition* terrain);
    ~WidgetHeightMap();

    void setBrushMode(HeightMapBrushMode mode);
    void setBrushSize(double size);
    void setBrushSmoothing(double smoothing);
    void setBrushStrength(double smoothing);

    QString getMemoryStats();

public slots:
    void revert();

signals:
    void heightmapChanged();

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

    void timerEvent(QTimerEvent* event);

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

private:
    void updateVertexInfo();

private:
    TerrainDefinition* _terrain;
    Renderer* _renderer;
    _VertexInfo* _vertices;

    bool _dirty;
    qint64 _memory_stats;

    double _water_height;
    bool _water;
    bool _wireframe;

    double _average_frame_time;

    int _last_brush_action;
    int _last_mouse_x;
    int _last_mouse_y;
    QDateTime _last_time;
    bool _mouse_moved;

    CameraDefinition* _top_camera;
    CameraDefinition* _temp_camera;
    CameraDefinition* _current_camera;
    double _zoom;

    double _brush_x;
    double _brush_z;
    HeightMapBrushMode _brush_mode;
    double _brush_size;
    double _brush_smoothing;
    double _brush_strength;
    NoiseGenerator* _brush_noise;
};

#endif
