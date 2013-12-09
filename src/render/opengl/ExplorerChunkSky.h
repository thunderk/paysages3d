#ifndef EXPLORERCHUNKSKY_H
#define EXPLORERCHUNKSKY_H

#include "opengl_global.h"

#include "BaseExplorerChunk.h"

#include "Vector3.h"

namespace paysages {
namespace opengl {

enum SkyboxOrientation
{
    SKYBOX_NORTH,
    SKYBOX_SOUTH,
    SKYBOX_WEST,
    SKYBOX_EAST,
    SKYBOX_TOP,
    SKYBOX_BOTTOM
};

class OPENGLSHARED_EXPORT ExplorerChunkSky:public BaseExplorerChunk
{
public:
    ExplorerChunkSky(SoftwareRenderer* renderer, double size, SkyboxOrientation orientation);

    void onCameraEvent(CameraDefinition* camera);
    void onRenderEvent(QGLWidget* widget);
    double getDisplayedSizeHint(CameraDefinition* camera);
    Color getTextureColor(double x, double y);

private:
    SkyboxOrientation _orientation;
    double _box_size;
    Vector3 _center;
};

}
}

#endif // EXPLORERCHUNKSKY_H
