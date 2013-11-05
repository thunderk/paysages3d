#include "ExplorerChunkTerrain.h"

#include <cmath>
#include <GL/gl.h>
#include "rendering/camera.h"
#include "rendering/renderer.h"

ExplorerChunkTerrain::ExplorerChunkTerrain(Renderer* renderer, double x, double z, double size, int nbchunks, double water_height) : BaseExplorerChunk(renderer)
{
    _startx = x;
    _startz = z;
    _size = size;
    _overall_step = size * (double) nbchunks;

    _distance_to_camera = 0.0;

    _water_height = water_height;
    _overwater = false;

    _tessellation_max_size = 32;
    _tessellation = new double[(_tessellation_max_size + 1) * (_tessellation_max_size + 1)];
    _tessellation_current_size = 0;
    _tessellation_step = _size / (double) _tessellation_max_size;

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
    _overwater = false;
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
                    double height = renderer->terrain->getHeight(renderer, _startx + _tessellation_step * (double) i, _startz + _tessellation_step * (double) j, 1);
                    if (height >= _water_height)
                    {
                        _overwater = true;
                    }
                    _tessellation[j * (_tessellation_max_size + 1) + i] = height;
                }
            }
        }

        _lock_data.lock();
        _tessellation_current_size = new_tessellation_size;
        _lock_data.unlock();

        if (_tessellation_current_size < 4 && _tessellation_current_size < _tessellation_max_size)
        {
            onMaintainEvent();
        }

        return true;
    }
    else
    {
        return false;
    }
}

void ExplorerChunkTerrain::onCameraEvent(CameraDefinition* camera)
{
    Vector3 camera_location = cameraGetLocation(camera);

    // Handle position
    _lock_data.lock();
    if (camera_location.x > _startx + _overall_step * 0.5)
    {
        _startx += _overall_step;
        askReset();
    }
    if (camera_location.z > _startz + _overall_step * 0.5)
    {
        _startz += _overall_step;
        askReset();
    }
    if (camera_location.x < _startx - _overall_step * 0.5)
    {
        _startx -= _overall_step;
        askReset();
    }
    if (camera_location.z < _startz - _overall_step * 0.5)
    {
        _startz -= _overall_step;
        askReset();
    }

    _distance_to_camera = v3Norm(v3Sub(getCenter(), camera_location));

    _lock_data.unlock();
}

void ExplorerChunkTerrain::onRenderEvent(QGLWidget*)
{
    _lock_data.lock();
    int tessellation_size = _tessellation_current_size;
    double tsize = 1.0 / (double) _tessellation_max_size;
    _lock_data.unlock();

    if (tessellation_size <= 1 or not _overwater)
    {
        return;
    }

    int tessellation_inc = _tessellation_max_size / (double) tessellation_size;
    for (int j = 0; j < _tessellation_max_size; j += tessellation_inc)
    {
        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= _tessellation_max_size; i += tessellation_inc)
        {
            glTexCoord2d(tsize * (double) i, tsize * (double) j);
            glVertex3d(_startx + _tessellation_step * (double) i, _tessellation[j * (_tessellation_max_size + 1) + i], _startz + _tessellation_step * (double) j);
            glTexCoord2d(tsize * (double) i, tsize * (double) (j + tessellation_inc));
            glVertex3d(_startx + _tessellation_step * (double) i, _tessellation[(j + tessellation_inc) * (_tessellation_max_size + 1) + i], _startz + _tessellation_step * (double) (j + tessellation_inc));
        }
        glEnd();
    }
}

double ExplorerChunkTerrain::getDisplayedSizeHint(CameraDefinition* camera)
{
    double distance;
    Vector3 center;

    if (not _overwater)
    {
        return -1000.0;
    }

    center = getCenter();

    if (cameraIsBoxInView(camera, center, _size, 40.0, _size))
    {
        distance = _distance_to_camera;
        distance = distance < 0.1 ? 0.1 : distance;
        return (int) ceil(120.0 - distance / 1.5);
    }
    else
    {
        return -800.0;
    }
}

Color ExplorerChunkTerrain::getTextureColor(double x, double y)
{
    Vector3 location = {_startx + x * _size, 0.0, _startz + y * _size};
    return renderer()->terrain->getFinalColor(renderer(), location, 0.01);
}

Vector3 ExplorerChunkTerrain::getCenter()
{
    Vector3 result;

    result.x = _startx + _size / 2.0;
    result.y = 0.0;
    result.z = _startz + _size / 2.0;

    return result;
}
