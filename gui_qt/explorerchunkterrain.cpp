#include "explorerchunkterrain.h"

#include <math.h>
#include "baseexplorerchunk.h"
#include "../lib_paysages/camera.h"

ExplorerChunkTerrain::ExplorerChunkTerrain(Renderer* renderer, float x, float z, float size, int nbchunks) : BaseExplorerChunk(renderer)
{
    _startx = x;
    _startz = z;
    _size = size;
    _overall_step = size * (float)nbchunks;
    
    _tessellation_max_size = 32;
    _tessellation = new float[(_tessellation_max_size + 1) * (_tessellation_max_size + 1)];
    _tessellation_current_size = 0;
    _tessellation_step = _size / (float)_tessellation_max_size;
    
    setMaxTextureSize(128);
    
    maintain();
}

ExplorerChunkTerrain::~ExplorerChunkTerrain()
{
    _lock_data.lock();
    delete [] _tessellation;
    _lock_data.unlock();
}

void ExplorerChunkTerrain::onResetEvent()
{
    _tessellation_current_size = 0;
}

bool ExplorerChunkTerrain::onMaintainEvent()
{
    Renderer* renderer = this->renderer();
    
    // Improve heightmap resolution
    if (_tessellation_current_size < _tessellation_max_size)
    {
        int new_tessellation_size = _tessellation_current_size ? _tessellation_current_size * 4 : 2;
        int old_tessellation_inc = _tessellation_current_size ? _tessellation_max_size / _tessellation_current_size : 1;
        int new_tessellation_inc = _tessellation_max_size / new_tessellation_size;
        for (int j = 0; j <= _tessellation_max_size; j += new_tessellation_inc)
        {
            for (int i = 0; i <= _tessellation_max_size; i += new_tessellation_inc)
            {
                if (_tessellation_current_size == 0 || i % old_tessellation_inc != 0 || j % old_tessellation_inc != 0)
                {
                    float height = renderer->getTerrainHeight(renderer, _startx + _tessellation_step * (float)i, _startz + _tessellation_step * (float)j);
                    _tessellation[j * (_tessellation_max_size + 1) + i] = height;
                }
            }
        }

        _lock_data.lock();
        _tessellation_current_size = new_tessellation_size;
        _lock_data.unlock();

        return true;
    }
    else
    {
        return false;
    }
}

void ExplorerChunkTerrain::onCameraEvent(CameraDefinition* camera)
{
    // Handle position
    _lock_data.lock();
    if (camera->location.x > _startx + _overall_step * 0.5)
    {
        _startx += _overall_step;
        askReset();
    }
    if (camera->location.z > _startz + _overall_step * 0.5)
    {
        _startz += _overall_step;
        askReset();
    }
    if (camera->location.x < _startx - _overall_step * 0.5)
    {
        _startx -= _overall_step;
        askReset();
    }
    if (camera->location.z < _startz - _overall_step * 0.5)
    {
        _startz -= _overall_step;
        askReset();
    }
    _lock_data.unlock();
}

void ExplorerChunkTerrain::onRenderEvent(QGLWidget* widget)
{
    _lock_data.lock();
    int tessellation_size = _tessellation_current_size;
    float tsize = 1.0 / (float)_tessellation_max_size;
    _lock_data.unlock();
    
    if (tessellation_size <= 1)
    {
        return;
    }

    int tessellation_inc = _tessellation_max_size / (float)tessellation_size;
    for (int j = 0; j < _tessellation_max_size; j += tessellation_inc)
    {
        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= _tessellation_max_size; i += tessellation_inc)
        {
            glTexCoord2d(tsize * (float)i, tsize * (float)j);
            glVertex3d(_startx + _tessellation_step * (float)i, _tessellation[j * (_tessellation_max_size + 1) + i], _startz + _tessellation_step * (float)j);
            glTexCoord2d(tsize * (float)i, tsize * (float)(j + tessellation_inc));
            glVertex3d(_startx + _tessellation_step * (float)i, _tessellation[(j + tessellation_inc) * (_tessellation_max_size + 1) + i], _startz + _tessellation_step * (float)(j + tessellation_inc));
        }
        glEnd();
    }
}

float ExplorerChunkTerrain::getDisplayedSizeHint(CameraDefinition* camera)
{
    float distance;
    Vector3 center;

    center = getCenter();

    if (cameraIsBoxInView(camera, center, _size, 40.0, _size))
    {
        distance = v3Norm(v3Sub(camera->location, center));
        distance = distance < 0.1 ? 0.1 : distance;
        return (int)ceil(120.0 - distance / 1.5);
    }
    else
    {
        return -800.0;
    }
}

Color ExplorerChunkTerrain::getTextureColor(float x, float y)
{
    Vector3 location = {_startx + x * _size, 0.0, _startz + y * _size};
    return renderer()->applyTextures(renderer(), location, 0.01);
}

Vector3 ExplorerChunkTerrain::getCenter()
{
    Vector3 result;
    
    result.x = _startx + _size / 2.0;
    result.y = 0.0;
    result.z = _startz + _size / 2.0;
    
    return result;
}
