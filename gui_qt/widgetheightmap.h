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

protected:
    void keyPressEvent(QKeyEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    
private:
    double _average_frame_time;
    int _last_mouse_x;
    int _last_mouse_y;
};

#endif
