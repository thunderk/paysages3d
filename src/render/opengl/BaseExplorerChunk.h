#ifndef BASEEXPLORERCHUNK_H
#define BASEEXPLORERCHUNK_H

#include "opengl_global.h"

#include <QMutex>
#include "Color.h"

class QImage;
class QGLWidget;
class Renderer;

namespace paysages {
namespace opengl {

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

    inline Renderer* renderer() {return _renderer;}

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
    unsigned int _texture_id;
    bool _texture_changed;
    int _texture_current_size;
    int _texture_max_size;
};

}
}

#endif // BASEEXPLORERCHUNK_H
