#include "wandererchunk.h"

#include <QMutex>
#include <QImage>
#include <QColor>
#include <QGLWidget>
#include <math.h>
#include "../lib_paysages/camera.h"
#include "../lib_paysages/color.h"
#include "../lib_paysages/euclid.h"
#include "../lib_paysages/tools.h"

WandererChunk::WandererChunk(Renderer* renderer, double x, double z, double size)
{
    _renderer = renderer;
    
    _startx = x;
    _startz = z;
    _size = size;
    
    priority = 0.0;
    
    _tessellation_max_size = 32;
    _tessellation = new double[(_tessellation_max_size + 1) * (_tessellation_max_size + 1)];
    _tessellation_current_size = 0;
    _tessellation_step = _size / (double)_tessellation_max_size;
    
    _texture_max_size = 128;
    _texture_current_size = 0;
    _texture = new QImage(1, 1, QImage::Format_ARGB32);
    _texture_id = 0;
    _texture_changed = false;
    
    maintain();
}

WandererChunk::~WandererChunk()
{
    _lock_data.lock();
    delete _texture;
    delete [] _tessellation;
    _lock_data.unlock();
}

void WandererChunk::render(QGLWidget* widget)
{
    _lock_data.lock();
    
    if (_texture_changed)
    {
        _texture_changed = false;
        if (_texture_id)
        {
            widget->deleteTexture(_texture_id);
        }
        _texture_id = widget->bindTexture(*_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, _texture_id);
    }

    int tessellation_size = _tessellation_current_size;
    int tessellation_inc = _tessellation_max_size / (double)tessellation_size;
    double tsize = 1.0 / (double)_tessellation_max_size;
    
    _lock_data.unlock();
    
    for (int j = 0; j < _tessellation_max_size; j += tessellation_inc)
    {
        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= _tessellation_max_size; i += tessellation_inc)
        {
            glTexCoord2d(tsize * (double)i, 1.0 - tsize * (double)j);
            glVertex3d(_startx + _tessellation_step * (double)i, _tessellation[j * (_tessellation_max_size + 1) + i], _startz + _tessellation_step * (double)j);
            glTexCoord2d(tsize * (double)i, 1.0 - tsize * (double)(j + tessellation_inc));
            glVertex3d(_startx + _tessellation_step * (double)i, _tessellation[(j + tessellation_inc) * (_tessellation_max_size + 1) + i], _startz + _tessellation_step * (double)(j + tessellation_inc));
        }
        glEnd();
    }
}

void WandererChunk::updatePriority(CameraDefinition* camera)
{
    // Compute new priority
    _lock_data.lock();
    if (_tessellation_current_size == _tessellation_max_size && _texture_current_size == _texture_max_size)
    {
        priority = -1000.0;
    }
    else
    {
        double distance, wanted_size;
        Vector3 center;
        
        center = getCenter();
        
        distance = v3Norm(v3Sub(camera->location, center));
        distance = distance < 0.1 ? 0.1 : distance;
        wanted_size = (int)ceil(120.0 - distance / 3.0);
        
        priority = wanted_size - _texture_current_size;
        if (_texture_current_size == 1)
        {
            priority += 100.0;
        }
        else if (distance < 15.0 && _texture_current_size < _texture_max_size)
        {
            priority += 75.0;
        }
        else if (distance < 30.0 && _texture_current_size < _texture_max_size / 2)
        {
            priority += 50.0;
        }
        
        if (!cameraIsBoxInView(camera, center, _size, _size, 40.0))
        {
            priority -= 100.0;
        }
    }
    _lock_data.unlock();
}

bool WandererChunk::maintain()
{
    if (_tessellation_current_size < _tessellation_max_size || _texture_current_size < _texture_max_size)
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
                        double height = _renderer->getTerrainHeight(_renderer, _startx + _tessellation_step * (double)i, _startz + _tessellation_step * (double)j);
                        _tessellation[j * (_tessellation_max_size + 1) + i] = height;
                    }
                }
            }
            
            _lock_data.lock();
            _tessellation_current_size = new_tessellation_size;
            _lock_data.unlock();
        }

        // Improve texture resolution
        if (_texture_current_size < _texture_max_size)
        {
            int new_texture_size = _texture_current_size ? _texture_current_size * 2 : 1;
            double step_size = _texture_current_size ? _size / (double)(new_texture_size - 1) : 0.1;
            QImage* new_image = new QImage(new_texture_size, new_texture_size, QImage::Format_ARGB32);
            for (int j = 0; j < new_texture_size; j++)
            {
                for (int i = 0; i < new_texture_size; i++)
                {
                    Vector3 location = {_startx + step_size * (double)i, 0.0, _startz + step_size * (double)j};
                    Color color = _renderer->applyTextures(_renderer, location, step_size);
                    new_image->setPixel(i, j, colorTo32BitRGBA(&color));
                }
            }
            
            _lock_data.lock();
            delete _texture;
            _texture = new_image;
            _texture_current_size = new_texture_size;
            _texture_changed = true;
            _lock_data.unlock();
        }

        return true;
    }
    else
    {
        return false;
    }
}

Vector3 WandererChunk::getCenter()
{
    Vector3 result;
    
    result.x = _startx + _size / 2.0;
    result.y = 0.0;
    result.z = _startz + _size / 2.0;
    
    return result;
}
