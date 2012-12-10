#ifndef _PAYSAGES_QT_WIDGETHEIGHTMAP_H_
#define _PAYSAGES_QT_WIDGETHEIGHTMAP_H_

#include <QGLWidget>
#include <QDateTime>
#include "../lib_paysages/euclid.h"
#include "../lib_paysages/terrain/public.h"

typedef struct
{
    Vector3 point;
    Vector3 normal;
} _VertexInfo;

typedef enum
{
    HEIGHTMAP_BRUSH_RAISE = 0,
    HEIGHTMAP_BRUSH_SMOOTH = 1
} HeightMapBrushMode;

class WidgetHeightMap : public QGLWidget
{
    Q_OBJECT
public:
    WidgetHeightMap(QWidget* parent, TerrainDefinition* terrain);
    ~WidgetHeightMap();

    void setHorizontalViewAngle(double angle_h);
    void setVerticalViewAngle(double angle_v);
    void setBrushMode(HeightMapBrushMode mode);
    void setBrushSize(double size);
    void setBrushSmoothing(double smoothing);
    void setBrushStrength(double smoothing);

public slots:
    void revert();

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

    void timerEvent(QTimerEvent* event);

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

private:
    void updateVertexInfo();

private:
    TerrainDefinition* _terrain;
    _VertexInfo* _vertices;

    bool _dirty;

    double _average_frame_time;

    int _last_brush_action;
    int _last_mouse_x;
    int _last_mouse_y;
    QDateTime _last_time;
    bool _mouse_moved;

    double _angle_h;
    double _angle_v;

    double _brush_x;
    double _brush_z;
    HeightMapBrushMode _brush_mode;
    double _brush_size;
    double _brush_smoothing;
    double _brush_strength;
    NoiseGenerator* _brush_noise;
};

#endif
