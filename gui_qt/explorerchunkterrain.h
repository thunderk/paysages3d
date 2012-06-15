#ifndef _PAYSAGES_QT_EXPLORERCHUNKTERRAIN_H_
#define _PAYSAGES_QT_EXPLORERCHUNKTERRAIN_H_

#include "baseexplorerchunk.h"
#include "../lib_paysages/renderer.h"
#include "../lib_paysages/euclid.h"

class ExplorerChunkTerrain:public BaseExplorerChunk
{
public:
    ExplorerChunkTerrain(Renderer* renderer, float x, float z, float size, int nbchunks);
    ~ExplorerChunkTerrain();
    
    void onCameraEvent(CameraDefinition* camera);
    void onResetEvent();
    bool onMaintainEvent();
    void onRenderEvent(QGLWidget* widget);
    float getDisplayedSizeHint(CameraDefinition* camera);
    Color getTextureColor(float x, float y);
    
private:
    Vector3 getCenter();
    
    float _startx;
    float _startz;
    float _size;
    float _overall_step;
    
    float* _tessellation;
    int _tessellation_max_size;
    int _tessellation_current_size;
    float _tessellation_step;
    
};

#endif
