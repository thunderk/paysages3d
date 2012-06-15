#ifndef _PAYSAGES_QT_EXPLORERCHUNKSKY_H_
#define _PAYSAGES_QT_EXPLORERCHUNKSKY_H_

#include "baseexplorerchunk.h"
#include "../lib_paysages/renderer.h"
#include "../lib_paysages/euclid.h"
#include "../lib_paysages/sky.h"

enum SkyboxOrientation
{
    SKYBOX_NORTH,
    SKYBOX_SOUTH,
    SKYBOX_WEST,
    SKYBOX_EAST,
    SKYBOX_TOP,
    SKYBOX_BOTTOM
};

class ExplorerChunkSky:public BaseExplorerChunk
{
public:
    ExplorerChunkSky(Renderer* renderer, SkyDefinition* sky, float size, SkyboxOrientation orientation);
    
    void onRenderEvent(QGLWidget* widget);
    float getDisplayedSizeHint(CameraDefinition* camera);
    Color getTextureColor(float x, float y);
    
private:
    SkyDefinition* _sky;
    SkyboxOrientation _orientation;
    float _box_size;
    
};

#endif
