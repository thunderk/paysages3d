#ifndef _PAYSAGES_QT_WIDGETWANDERER_H_
#define _PAYSAGES_QT_WIDGETWANDERER_H_

#include <QGLWidget>
#include "../lib_paysages/camera.h"
#include "../lib_paysages/terrain.h"
#include "../lib_paysages/water.h"

class WidgetWanderer : public QGLWidget
{
    Q_OBJECT
public:
    WidgetWanderer(QWidget* parent, CameraDefinition* camera);

public slots:
    void resetCamera();
    void validateCamera();

protected:
    void keyPressEvent(QKeyEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

private:
    CameraDefinition _current_camera;
    CameraDefinition* _base_camera;

    TerrainDefinition terrain;
    WaterDefinition water;

    int last_mouse_x;
    int last_mouse_y;
};

#endif
