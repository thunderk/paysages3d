#include "ExplorerChunkTerrain.h"

#include OPENGL_FUNCTIONS_INCLUDE
#include <cmath>
#include <QImage>
#include <QOpenGLTexture>
#include "ColorProfile.h"
#include "CameraDefinition.h"
#include "OpenGLRenderer.h"
#include "TerrainRenderer.h"

ExplorerChunkTerrain::ExplorerChunkTerrain(OpenGLRenderer* renderer, double x, double z, double size, int nbchunks, double water_height):
    _renderer(renderer)
{
    _color_profile = new ColorProfile;

    priority = 0.0;
    _reset_needed = false;

    _texture = new QImage(1, 1, QImage::Format_ARGB32);
    texture = new QOpenGLTexture(*_texture);
    _texture_changed = false;
    _texture_current_size = 0;
    _texture_max_size = 0;

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
    delete _color_profile;
    delete _texture;
    delete texture;
    delete [] _tessellation;
    _lock_data.unlock();
}

bool ExplorerChunkTerrain::maintain()
{
    bool subchanged;

    _lock_data.lock();
    if (_reset_needed)
    {
        _reset_needed = false;
        _texture_current_size = 0;
        _tessellation_current_size = 0;
        _overwater = false;
    }
    _lock_data.unlock();

    subchanged = onMaintainEvent();

    // Improve texture resolution
    if (_texture_current_size < _texture_max_size)
    {
        int new_texture_size = _texture_current_size ? _texture_current_size * 2 : 1;
        QImage* new_image = new QImage(_texture->scaled(new_texture_size + 1, new_texture_size + 1, Qt::IgnoreAspectRatio, Qt::FastTransformation));
        for (int j = 0; j <= new_texture_size; j++)
        {
            for (int i = 0; i <= new_texture_size; i++)
            {
                if (_texture_current_size <= 1 || i % 2 != 0 || j % 2 != 0)
                {
                    Color color = getTextureColor((double)i / (double)new_texture_size, (double)j / (double)new_texture_size);
                    color = _color_profile->apply(color);
                    color.normalize();
                    new_image->setPixel(i, j, color.to32BitBGRA());
                }
            }
        }

        _lock_data.lock();
        delete _texture;
        _texture = new_image;
        _texture_current_size = new_texture_size;
        _texture_changed = true;
        _lock_data.unlock();

        /*if (_texture_current_size < 4 && _texture_current_size < _texture_max_size)
        {
            maintain();
        }*/

        return true;
    }
    else
    {
        return subchanged;
    }
}

bool ExplorerChunkTerrain::onMaintainEvent()
{
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
                    double height = _renderer->getTerrainRenderer()->getHeight(_startx + _tessellation_step * (double) i, _startz + _tessellation_step * (double) j, 1);
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

void ExplorerChunkTerrain::updatePriority(CameraDefinition* camera)
{
    if (_reset_needed || (_texture_max_size > 1 && _texture_current_size <= 1))
    {
        priority = 1000.0;
    }
    else if (_texture_current_size == _texture_max_size)
    {
        priority = -1000.0;
    }
    else
    {
        priority = getDisplayedSizeHint(camera) - _texture_current_size;
    }

    Vector3 camera_location = camera->getLocation();

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

    _distance_to_camera = getCenter().sub(camera_location).getNorm();

    _lock_data.unlock();
}

void ExplorerChunkTerrain::render(OpenGLFunctions* functions)
{
    // Put texture in place
    _lock_data.lock();
    if (_texture_changed)
    {
        _texture_changed = false;
        texture->destroy();
        // TODO Only do the scale if not power-of-two textures are unsupported by GPU
        texture->setData(_texture->scaled(_texture_current_size, _texture_current_size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        //texture->setData(*_texture);
        texture->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::ClampToEdge);
        texture->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::ClampToEdge);
    }
    texture->bind();
    _lock_data.unlock();

    // Delegate poly rendering to subclass
    if (!_reset_needed)
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
            functions->glBegin(GL_QUAD_STRIP);
            for (int i = 0; i <= _tessellation_max_size; i += tessellation_inc)
            {
                functions->glTexCoord2d(tsize * (double) i, tsize * (double) j);
                functions->glVertex3d(_startx + _tessellation_step * (double) i, _tessellation[j * (_tessellation_max_size + 1) + i], _startz + _tessellation_step * (double) j);
                functions->glTexCoord2d(tsize * (double) i, tsize * (double) (j + tessellation_inc));
                functions->glVertex3d(_startx + _tessellation_step * (double) i, _tessellation[(j + tessellation_inc) * (_tessellation_max_size + 1) + i], _startz + _tessellation_step * (double) (j + tessellation_inc));
            }
            functions->glEnd();
        }
    }
}

void ExplorerChunkTerrain::askReset()
{
    _reset_needed = true;
}

void ExplorerChunkTerrain::setMaxTextureSize(int size)
{
    _texture_max_size = size;
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

    if (camera->isBoxInView(center, _size, 40.0, _size))
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
    return _renderer->getTerrainRenderer()->getFinalColor(location, 0.01);
}

Vector3 ExplorerChunkTerrain::getCenter()
{
    Vector3 result;

    result.x = _startx + _size / 2.0;
    result.y = 0.0;
    result.z = _startz + _size / 2.0;

    return result;
}
