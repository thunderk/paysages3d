#include "ExplorerChunkTerrain.h"

#include OPENGL_FUNCTIONS_INCLUDE
#include <cmath>
#include <QImage>
#include "ColorProfile.h"
#include "CameraDefinition.h"
#include "OpenGLRenderer.h"
#include "TerrainRenderer.h"
#include "VertexArray.h"

ExplorerChunkTerrain::ExplorerChunkTerrain(OpenGLRenderer* renderer, double x, double z, double size, int nbchunks, double water_height):
    _renderer(renderer)
{
    _color_profile = new ColorProfile(ColorProfile::TONE_MAPPING_REIHNARD, 2.0);

    priority = 0.0;
    _reset_needed = false;

    _texture = new QImage(1, 1, QImage::Format_RGBA8888);
    texture_id = 0;
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

    tessellation_count = 33;
    tessellated = new VertexArray<TerrainVertex>();
    tessellated->setGridSize(tessellation_count);
    tessellated->setAutoGridIndices(tessellation_count);
    _tessellation_max_size = tessellation_count - 1;
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
    delete tessellated;
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
                    //color = _color_profile->apply(color);
                    color.normalize();
                    new_image->setPixel(i, j, color.to32BitRGBA());
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
        float internal_step = 1.0f / (float)_tessellation_max_size;
        for (int j = 0; j <= _tessellation_max_size; j += new_tessellation_inc)
        {
            for (int i = 0; i <= _tessellation_max_size; i += new_tessellation_inc)
            {
                if (_tessellation_current_size == 0 || i % old_tessellation_inc != 0 || j % old_tessellation_inc != 0)
                {
                    double x = _startx + _tessellation_step * (float)i;
                    double z = _startz + _tessellation_step * (float)j;

                    double height = _renderer->getTerrainRenderer()->getHeight(x, z, 1);
                    if (height >= _water_height)
                    {
                        _overwater = true;
                    }

                    TerrainVertex v;

                    v.uv[0] = internal_step * (float)i;
                    v.uv[1] = internal_step * (float)j;

                    v.location[0] = x;
                    v.location[1] = height;
                    v.location[2] = z;

                    tessellated->setGridVertex(tessellation_count, i, j, v);
                }
            }
        }

        _lock_data.lock();
        _tessellation_current_size = new_tessellation_size;
        tessellated->setAutoGridIndices(tessellation_count, new_tessellation_inc);
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

void ExplorerChunkTerrain::render(QOpenGLShaderProgram* program, OpenGLFunctions* functions)
{
    // Put texture in place
    _lock_data.lock();
    if (_texture_changed)
    {
        _texture_changed = false;

        // TODO Only do the scale if not power-of-two textures are unsupported by GPU
        QImage tex = _texture->scaled(_texture_current_size, _texture_current_size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        if (texture_id == 0)
        {
            GLuint texid;
            functions->glGenTextures(1, &texid);
            texture_id = texid;
        }

        functions->glBindTexture(GL_TEXTURE_2D, texture_id);
        functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        functions->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width(), tex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());
    }
    _lock_data.unlock();

    // Render tessellated mesh
    if (!_reset_needed)
    {
        _lock_data.lock();
        int tessellation_size = _tessellation_current_size;
        _lock_data.unlock();

        if (tessellation_size <= 1 or not _overwater)
        {
            return;
        }

        _lock_data.lock(); // TEMP
        // TEMP
        functions->glActiveTexture(GL_TEXTURE0 + 3);
        functions->glBindTexture(GL_TEXTURE_2D, texture_id);
        program->setUniformValue("groundTexture", 3);

        tessellated->render(program, functions);
        _lock_data.unlock(); // TEMP
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
