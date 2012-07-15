#ifndef _PAYSAGES_QT_WIDGETHEIGHTMAP_H_
#define _PAYSAGES_QT_WIDGETHEIGHTMAP_H_

#include <QGLWidget>
#include "../lib_paysages/euclid.h"
#include "../lib_paysages/heightmap.h"

typedef struct
{
    Vector3 point;
    Vector3 normal;
} _VertexInfo;

class WidgetHeightMap : public QGLWidget
{
    Q_OBJECT
public:
    WidgetHeightMap(QWidget* parent, HeightMap* heightmap);
    ~WidgetHeightMap();
    
    void setHorizontalViewAngle(double angle_h);
    void setVerticalViewAngle(double angle_v);
    void setBrushSize(double size);
    void setBrushSmoothing(double smoothing);
    
public slots:
    void resetToTerrain();

protected:
    void keyPressEvent(QKeyEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    
private:
    void updateVertexInfo();

private:
    HeightMap* _heightmap;
    _VertexInfo* _vertexes;
    
    bool _dirty;
    
    double _average_frame_time;
    
    int _last_mouse_x;
    int _last_mouse_y;
    
    double _angle_h;
    double _angle_v;
    
    double _brush_x;
    double _brush_z;
    double _brush_size;
    double _brush_smoothing;
};

#endif
