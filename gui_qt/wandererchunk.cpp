#include "wandererchunk.h"

#include <QMutex>
#include <QImage>
#include <QColor>
#include <QGLWidget>
#include "../lib_paysages/color.h"
#include "../lib_paysages/tools.h"

WandererChunk::WandererChunk(double x, double z, double size)
{
    _startx = x;
    _startz = z;
    _dirty = 3;
    _chunksize = size;
    _nbsubchunks = 8;
    _subchunksize = size / (double)_nbsubchunks;
    _texture = new QImage(1, 1, QImage::Format_ARGB32);
    _texture->fill(QColor(0, 200, 0));
    _texture_id = 0;
    _need_texture_upload = true;
    _heightmap = new double[(_nbsubchunks + 1) * (_nbsubchunks + 1)];
    for (int j = 0; j <= _nbsubchunks; j++)
    {
        for (int i = 0; i <= _nbsubchunks; i++)
        {
            _heightmap[j * (_nbsubchunks + 1) + i] = -100.0;
        }
    }
}

WandererChunk::~WandererChunk()
{
    _lock.lock();
    delete _texture;
    delete [] _heightmap;
    _lock.unlock();
}

void WandererChunk::render(QGLWidget* widget)
{
    _lock.lock();
    
    if (_need_texture_upload)
    {
        _need_texture_upload = false;
        if (_texture_id)
        {
            widget->deleteTexture(_texture_id);
        }
        _texture_id = widget->bindTexture(*_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    glBindTexture(GL_TEXTURE_2D, _texture_id);

    double tsize = 1.0 / (double)_nbsubchunks;
    for (int j = 0; j < _nbsubchunks; j++)
    {
        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= _nbsubchunks; i++)
        {
            glTexCoord2d(tsize * (double)i, 1.0 - tsize * (double)j);
            glVertex3d(_startx + _subchunksize * (double)i, _heightmap[j * (_nbsubchunks + 1) + i], _startz + _subchunksize * (double)j);
            glTexCoord2d(tsize * (double)i, 1.0 - tsize * (double)(j + 1));
            glVertex3d(_startx + _subchunksize * (double)i, _heightmap[(j + 1) * (_nbsubchunks + 1) + i], _startz + _subchunksize * (double)(j + 1));
        }
        glEnd();
    }
    
    _lock.unlock();
}

bool WandererChunk::maintain(Renderer* renderer)
{
    _lock_dirty.lock();
    if (_dirty)
    {
        int dirty = _dirty--;
        _lock_dirty.unlock();
        
        // Compute heightmap
        if (dirty == 3)
        {
            double* new_heightmap = new double[(_nbsubchunks + 1) * (_nbsubchunks + 1)];
            for (int j = 0; j <= _nbsubchunks; j++)
            {
                for (int i = 0; i <= _nbsubchunks; i++)
                {
                    new_heightmap[j * (_nbsubchunks + 1) + i] = renderer->getTerrainHeight(renderer, _startx + _subchunksize * (double)i, _startz + _subchunksize * (double)j);
                }
            }
            _lock.lock();
            delete [] _heightmap;
            _heightmap = new_heightmap;
            _lock.unlock();
        }

        // Compute low-res texture
        if (dirty == 2)
        {
            int texture_size = 4;
            double step_size = _chunksize / (double)(texture_size - 1);
            QImage* new_image = new QImage(texture_size, texture_size, QImage::Format_ARGB32);
            for (int j = 0; j < texture_size; j++)
            {
                for (int i = 0; i < texture_size; i++)
                {
                    Vector3 location = {_startx + step_size * (double)i, 0.0, _startz + step_size * (double)j};
                    Color color = renderer->applyTextures(renderer, location, step_size);
                    new_image->setPixel(i, j, colorTo32BitRGBA(&color));
                }
            }
            _lock.lock();
            delete _texture;
            _texture = new_image;
            _need_texture_upload = true;
            _lock.unlock();
        }

        // Compute texture
        if (dirty == 1)
        {
            int texture_size = 32;
            double step_size = _chunksize / (double)(texture_size - 1);
            QImage* new_image = new QImage(texture_size, texture_size, QImage::Format_ARGB32);
            for (int j = 0; j < texture_size; j++)
            {
                for (int i = 0; i < texture_size; i++)
                {
                    Vector3 location = {_startx + step_size * (double)i, 0.0, _startz + step_size * (double)j};
                    Color color = renderer->applyTextures(renderer, location, step_size);
                    new_image->setPixel(i, j, colorTo32BitRGBA(&color));
                }
            }
            _lock.lock();
            delete _texture;
            _texture = new_image;
            _need_texture_upload = true;
            _lock.unlock();
        }
        
        return true;
    }
    else
    {
        _lock_dirty.unlock();
        
        return false;
    }
}

Vector3 WandererChunk::getCenter()
{
    Vector3 result;
    
    result.x = _startz + _chunksize / 2.0;
    result.y = 0.0;
    result.z = _startz + _chunksize / 2.0;
    
    return result;
}
