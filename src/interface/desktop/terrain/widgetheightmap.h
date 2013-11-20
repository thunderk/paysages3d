#ifndef _PAYSAGES_QT_WIDGETHEIGHTMAP_H_
#define _PAYSAGES_QT_WIDGETHEIGHTMAP_H_

#include <QGLWidget>
#include <QDateTime>
#include "terrain/paintingbrush.h"
#include "CameraDefinition.h"
#include "renderer.h"
#include "terrain/public.h"

typedef struct
{
    Vector3 point;
    Vector3 normal;
    int painted;
} _VertexInfo;

class WidgetHeightMap : public QGLWidget
{
    Q_OBJECT
public:
    WidgetHeightMap(QWidget* parent);
    ~WidgetHeightMap();

    void setTerrain(TerrainDefinition* terrain);
    void setBrush(PaintingBrush* brush);

public slots:
    void revert();
    void toggleWater(bool enabled);
    void toggleGrid(bool enabled);
    void togglePaintedArea(bool enabled);

signals:
    void heightmapChanged();

protected:
    void keyPressEvent(QKeyEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

    void timerEvent(QTimerEvent* event);

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

private:
    void scrollTopCamera(double dx, double dz);
    void zoomTopCamera(double dzoom);

    void updateVertexInfo();

private:
    TerrainDefinition* _terrain;
    Renderer* _renderer;
    _VertexInfo* _vertices;

    bool _dirty;

    double _water_height;
    bool _water;
    bool _wireframe;
    bool _painted_area;

    double _average_frame_time;

    int _last_brush_action;
    int _last_mouse_x;
    int _last_mouse_y;
    QDateTime _last_time;
    bool _mouse_moved;

    double _target_x;
    double _target_z;
    int _last_update_x;
    int _last_update_z;

    CameraDefinition* _top_camera;
    CameraDefinition* _temp_camera;
    CameraDefinition* _current_camera;
    double _zoom;

    PaintingBrush* _brush;
    double _brush_x;
    double _brush_z;
};

#endif
