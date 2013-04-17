#ifndef _PAYSAGES_QT_BASEEXPLORERCHUNK_H_
#define _PAYSAGES_QT_BASEEXPLORERCHUNK_H_

#include <QMutex>
#include <QImage>
#include <QGLWidget>
#include "rendering/renderer.h"

class BaseExplorerChunk
{
public:
    virtual ~BaseExplorerChunk();

    bool maintain();
    void updatePriority(CameraDefinition* camera);
    void render(QGLWidget* widget);

    double priority;
protected:
    BaseExplorerChunk(Renderer* renderer);

    inline Renderer* renderer() {return _renderer;};

    void askReset();
    void setMaxTextureSize(int size);

    virtual void onCameraEvent(CameraDefinition* camera);
    virtual void onResetEvent();
    virtual bool onMaintainEvent();
    virtual void onRenderEvent(QGLWidget* widget);
    virtual double getDisplayedSizeHint(CameraDefinition* camera);
    virtual Color getTextureColor(double x, double y);

    QMutex _lock_data;

private:
    Renderer* _renderer;
    ColorProfile* _color_profile;

    bool _reset_needed;

    QImage* _texture;
    GLuint _texture_id;
    bool _texture_changed;
    int _texture_current_size;
    int _texture_max_size;
};

#endif
