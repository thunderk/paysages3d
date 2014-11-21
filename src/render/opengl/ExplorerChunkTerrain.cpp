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
    priority = 0.0;
    _reset_needed = false;

    interrupt = false;

    _texture = new QImage(1, 1, QImage::Format_RGBA8888);
    texture_id = 0;
    _texture_changed = false;
    _texture_current_size = 0;
    _texture_wanted_size = 0;
    _texture_max_size = 256;

    _startx = x;
    _startz = z;
    _size = size;
    _overall_step = size * (double) nbchunks;

    distance_to_camera = 0.0;

    _water_height = water_height;
    overwater = false;

    tessellation_count = 33;
    tessellated = new VertexArray<TerrainVertex>();
    tessellated->setGridSize(tessellation_count);
    tessellated->setAutoGridIndices(tessellation_count);
    _tessellation_max_size = tessellation_count - 1;
    _tessellation_current_size = 0;
    _tessellation_step = _size / (double) _tessellation_max_size;

    maintain();
}

ExplorerChunkTerrain::~ExplorerChunkTerrain()
{
    _lock_data.lock();
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
        overwater = false;
    }
    _lock_data.unlock();

    // Improve heightmap resolution
    if (_tessellation_current_size < _tessellation_max_size)
    {
        while (_tessellation_current_size < _tessellation_max_size)
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

                        double height = _renderer->getTerrainRenderer()->getHeight(x, z, true);
                        if (height >= _water_height)
                        {
                            overwater = true;
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
                if (interrupt or _reset_needed)
                {
                    return false;
                }
            }

            _lock_data.lock();
            _tessellation_current_size = new_tessellation_size;
            tessellated->setAutoGridIndices(tessellation_count, new_tessellation_inc);
            _lock_data.unlock();

            if (_tessellation_current_size >= 4)
            {
                break;
            }
        }
        subchanged = true;
    }
    else
    {
        subchanged = false;
    }

    // Improve texture resolution
    if (_texture_current_size < _texture_wanted_size)
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
                    color.normalize();
                    new_image->setPixel(i, j, color.to32BitRGBA());
                }
            }

            if (interrupt or _reset_needed)
            {
                return false;
            }
        }

        _lock_data.lock();
        delete _texture;
        _texture = new_image;
        _texture_current_size = new_texture_size;
        _texture_changed = true;
        _lock_data.unlock();

        return true;
    }
    else
    {
        return subchanged;
    }
}

void ExplorerChunkTerrain::updatePriority(CameraDefinition* camera)
{
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
    distance_to_camera = getCenter().sub(camera_location).getNorm();
    _lock_data.unlock();

    // Update wanted LOD
    if (not overwater)
    {
        _texture_wanted_size = 2;
    }
    else if (distance_to_camera < 50.0)
    {
        _texture_wanted_size = _texture_max_size;
    }
    else if (distance_to_camera < 100.0)
    {
        _texture_wanted_size = _texture_max_size / 4;
    }
    else if (distance_to_camera < 200.0)
    {
        _texture_wanted_size = _texture_max_size / 8;
    }
    else
    {
        _texture_wanted_size = 8;
    }

    // Update priority
    if (_reset_needed || (_texture_max_size > 1 && _texture_current_size <= 1))
    {
        priority = 1000.0;
    }
    else if (_texture_current_size == _texture_wanted_size)
    {
        priority = -1000.0;
    }
    else
    {
        priority = _texture_wanted_size / _texture_current_size;
    }

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

        if (tessellation_size <= 1 or not overwater)
        {
            return;
        }

        _lock_data.lock();
        // TEMP
        functions->glActiveTexture(GL_TEXTURE0 + 7);
        functions->glBindTexture(GL_TEXTURE_2D, texture_id);
        program->setUniformValue("groundTexture", 7);
        tessellated->render(program, functions);
        _lock_data.unlock();
    }
}

void ExplorerChunkTerrain::askReset()
{
    _reset_needed = true;
}

void ExplorerChunkTerrain::askInterrupt()
{
    interrupt = true;
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
