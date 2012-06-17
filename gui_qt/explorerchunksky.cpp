#include "explorerchunksky.h"

#include <math.h>
#include "baseexplorerchunk.h"
#include "../lib_paysages/camera.h"

ExplorerChunkSky::ExplorerChunkSky(Renderer* renderer, SkyDefinition* sky, double size, SkyboxOrientation orientation) : BaseExplorerChunk(renderer)
{
    _sky = sky;
    _box_size = size;
    _orientation = orientation;
    
    setMaxTextureSize(256);
    maintain();
}

void ExplorerChunkSky::onRenderEvent(QGLWidget* widget)
{
    double size = _box_size;
    Vector3 camera = renderer()->camera_location;

    glBegin(GL_QUADS);
    switch(_orientation)
    {
        case SKYBOX_NORTH:
            glTexCoord2d(0.0, 0.0);
            glVertex3d(camera.x - size, camera.y + size, camera.z - size);
            glTexCoord2d(0.0, 1.0);
            glVertex3d(camera.x - size, camera.y - size, camera.z - size);
            glTexCoord2d(1.0, 1.0);
            glVertex3d(camera.x + size, camera.y - size, camera.z - size);
            glTexCoord2d(1.0, 0.0);
            glVertex3d(camera.x + size, camera.y + size, camera.z - size);
            break;
        case SKYBOX_SOUTH:
            glTexCoord2d(0.0, 0.0);
            glVertex3d(camera.x + size, camera.y + size, camera.z + size);
            glTexCoord2d(0.0, 1.0);
            glVertex3d(camera.x + size, camera.y - size, camera.z + size);
            glTexCoord2d(1.0, 1.0);
            glVertex3d(camera.x - size, camera.y - size, camera.z + size);
            glTexCoord2d(1.0, 0.0);
            glVertex3d(camera.x - size, camera.y + size, camera.z + size);
            break;
        case SKYBOX_EAST:
            glTexCoord2d(0.0, 0.0);
            glVertex3d(camera.x + size, camera.y + size, camera.z - size);
            glTexCoord2d(0.0, 1.0);
            glVertex3d(camera.x + size, camera.y - size, camera.z - size);
            glTexCoord2d(1.0, 1.0);
            glVertex3d(camera.x + size, camera.y - size, camera.z + size);
            glTexCoord2d(1.0, 0.0);
            glVertex3d(camera.x + size, camera.y + size, camera.z + size);
            break;
        case SKYBOX_WEST:
            glTexCoord2d(0.0, 0.0);
            glVertex3d(camera.x - size, camera.y + size, camera.z + size);
            glTexCoord2d(0.0, 1.0);
            glVertex3d(camera.x - size, camera.y - size, camera.z + size);
            glTexCoord2d(1.0, 1.0);
            glVertex3d(camera.x - size, camera.y - size, camera.z - size);
            glTexCoord2d(1.0, 0.0);
            glVertex3d(camera.x - size, camera.y + size, camera.z - size);
            break;
        case SKYBOX_TOP:
            glTexCoord2d(0.0, 0.0);
            glVertex3d(camera.x - size, camera.y + size, camera.z + size);
            glTexCoord2d(0.0, 1.0);
            glVertex3d(camera.x - size, camera.y + size, camera.z - size);
            glTexCoord2d(1.0, 1.0);
            glVertex3d(camera.x + size, camera.y + size, camera.z - size);
            glTexCoord2d(1.0, 0.0);
            glVertex3d(camera.x + size, camera.y + size, camera.z + size);
            break;
        case SKYBOX_BOTTOM:
            glTexCoord2d(0.0, 0.0);
            glVertex3d(camera.x - size, camera.y - size, camera.z - size);
            glTexCoord2d(0.0, 1.0);
            glVertex3d(camera.x - size, camera.y - size, camera.z + size);
            glTexCoord2d(1.0, 1.0);
            glVertex3d(camera.x + size, camera.y - size, camera.z + size);
            glTexCoord2d(1.0, 0.0);
            glVertex3d(camera.x + size, camera.y - size, camera.z - size);
            break;
    }
    glEnd();
}

double ExplorerChunkSky::getDisplayedSizeHint(CameraDefinition* camera)
{
    return 1000.0;
}

Color ExplorerChunkSky::getTextureColor(double x, double y)
{
    Vector3 location;
    
    x -= 0.5;
    y -= 0.5;
    
    switch(_orientation)
    {
        case SKYBOX_NORTH:
            location.x = x;
            location.y = -y;
            location.z = -0.5;
            break;
        case SKYBOX_SOUTH:
            location.x = -x;
            location.y = -y;
            location.z = 0.5;
            break;
        case SKYBOX_EAST:
            location.x = 0.5;
            location.y = -y;
            location.z = x;
            break;
        case SKYBOX_WEST:
            location.x = -0.5;
            location.y = -y;
            location.z = -x;
            break;
        case SKYBOX_TOP:
            location.x = x;
            location.y = 0.5;
            location.z = -y;
            break;
        case SKYBOX_BOTTOM:
            location.x = x;
            location.y = -0.5;
            location.z = y;
            break;
    }
    return skyGetColor(_sky, renderer(), VECTOR_ZERO, v3Normalize(location));
}
