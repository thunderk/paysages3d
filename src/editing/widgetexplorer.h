#ifndef _PAYSAGES_QT_WIDGETEXPLORER_H_
#define _PAYSAGES_QT_WIDGETEXPLORER_H_

#include <QGLWidget>
#include "baseexplorerchunk.h"
#include "rendering/camera.h"
#include "rendering/renderer.h"

class WidgetExplorer : public QGLWidget
{
    Q_OBJECT
public:
    WidgetExplorer(QWidget* parent, CameraDefinition* camera, Renderer* renderer=0);
    ~WidgetExplorer();

    void performChunksMaintenance();

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
    void startRendering();
    void stopRendering();

    CameraDefinition* _current_camera;
    CameraDefinition* _base_camera;

    Renderer* _renderer;
    bool _renderer_created;
    bool _inited;
    bool _updated;

    QVector<BaseExplorerChunk*> _chunks;
    QList<BaseExplorerChunk*> _updateQueue;
    bool _alive;
    QMutex _lock_chunks;

    double _average_frame_time;
    int _quality;

    int _last_mouse_x;
    int _last_mouse_y;
};

#endif
