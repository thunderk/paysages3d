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

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

private:
    CameraDefinition* camera;
    TerrainDefinition terrain;
    WaterDefinition water;
};

#endif
