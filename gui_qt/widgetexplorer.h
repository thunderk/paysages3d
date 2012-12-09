#ifndef _PAYSAGES_QT_WIDGETEXPLORER_H_
#define _PAYSAGES_QT_WIDGETEXPLORER_H_

#include <QGLWidget>
#include "baseexplorerchunk.h"
#include "../lib_paysages/camera.h"
#include "../lib_paysages/water.h"
#include "../lib_paysages/renderer.h"
#include "../lib_paysages/textures.h"
#include "../lib_paysages/lighting.h"

class WidgetExplorer : public QGLWidget
{
    Q_OBJECT
public:
    WidgetExplorer(QWidget* parent, CameraDefinition* camera);
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
    void startThreads();
    void stopThreads();

    CameraDefinition _current_camera;
    CameraDefinition* _base_camera;

    Renderer _renderer;
    bool _updated;

    QVector<BaseExplorerChunk*> _chunks;
    QList<BaseExplorerChunk*> _updateQueue;
    bool _alive;
    QMutex _lock_chunks;

    WaterDefinition _water;
    TexturesDefinition _textures;
    LightingDefinition _lighting;

    double _average_frame_time;
    int _quality;

    int _last_mouse_x;
    int _last_mouse_y;
};

#endif
