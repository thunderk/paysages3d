#ifndef _PAYSAGES_QT_EXPLORERCHUNKTERRAIN_H_
#define _PAYSAGES_QT_EXPLORERCHUNKTERRAIN_H_

#include "baseexplorerchunk.h"
#include "rendering/renderer.h"
#include "rendering/tools/euclid.h"

class ExplorerChunkTerrain:public BaseExplorerChunk
{
public:
    ExplorerChunkTerrain(Renderer* renderer, double x, double z, double size, int nbchunks, double water_height);
    ~ExplorerChunkTerrain();

    void onCameraEvent(CameraDefinition* camera);
    void onResetEvent();
    bool onMaintainEvent();
    void onRenderEvent(QGLWidget* widget);
    double getDisplayedSizeHint(CameraDefinition* camera);
    Color getTextureColor(double x, double y);

private:
    Vector3 getCenter();

    double _startx;
    double _startz;
    double _size;
    double _overall_step;

    double _distance_to_camera;
    double _water_height;
    bool _overwater;

    double* _tessellation;
    int _tessellation_max_size;
    int _tessellation_current_size;
    double _tessellation_step;

};

#endif
