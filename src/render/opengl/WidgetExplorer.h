#ifndef WIDGETEXPLORER_H
#define WIDGETEXPLORER_H

#include "opengl_global.h"

#include <QGLWidget>

namespace paysages {
namespace opengl {

class OPENGLSHARED_EXPORT WidgetExplorer : public QGLWidget
{
    Q_OBJECT
public:
    WidgetExplorer(QWidget* parent, CameraDefinition* camera, Scenery* scenery);
    ~WidgetExplorer();

public slots:
    void resetCamera();
    void validateCamera();

protected:
    void keyPressEvent(QKeyEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    void timerEvent(QTimerEvent *event);

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

private:
    CameraDefinition* _current_camera;
    CameraDefinition* _base_camera;

    OpenGLRenderer* _renderer;

    double _average_frame_time;
    int _quality;

    int _last_mouse_x;
    int _last_mouse_y;
};

}
}

#endif // WIDGETEXPLORER_H
