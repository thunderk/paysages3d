#ifndef _PAYSAGES_QT_WIDGETHEIGHTMAP_H_
#define _PAYSAGES_QT_WIDGETHEIGHTMAP_H_

#include <QGLWidget>
#include "../lib_paysages/heightmap.h"

class WidgetHeightMap : public QGLWidget
{
    Q_OBJECT
public:
    WidgetHeightMap(QWidget* parent, HeightMap* heightmap);
    ~WidgetHeightMap();
    
    void setHorizontalViewAngle(double angle_h);
    void setVerticalViewAngle(double angle_v);
    
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
    HeightMap* _heightmap;
    
    double _average_frame_time;
    
    int _last_mouse_x;
    int _last_mouse_y;
    
    double _angle_h;
    double _angle_v;
};

#endif
