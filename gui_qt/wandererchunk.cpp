#include "wandererchunk.h"

#include <math.h>
#include <stdlib.h>
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
    _dirty = true;
    _chunksize = size;
    _nbsubchunks = 4;
    _subchunksize = size / (double)_nbsubchunks;
    _texture = new QImage(4, 4, QImage::Format_ARGB32);
    _texture->fill(QColor(0, 200, 0));
    _texture_id = 0;
    _need_texture_upload = true;
    _heightmap = (double*)malloc(sizeof(double) * (_nbsubchunks + 1) * (_nbsubchunks + 1));
    //_heightmap[0] = _heightmap[1] = _heightmap[2] = _heightmap[3] = toolsRandom();
}

WandererChunk::~WandererChunk()
{
    _lock.lock();
    delete _texture;
    free(_heightmap);
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
            glColor3f(1.0, 1.0, 1.0);
            glTexCoord2d(tsize * (double)i, 1.0 - tsize * (double)j);
            glVertex3d(_startx + _subchunksize * (double)i, _heightmap[j * (_nbsubchunks + 1) + i], _startz + _subchunksize * (double)j);
            glTexCoord2d(tsize * (double)i, 1.0 - tsize * (double)(j + 1));
            glVertex3d(_startx + _subchunksize * (double)i, _heightmap[(j + 1) * (_nbsubchunks + 1) + i], _startz + _subchunksize * (double)(j + 1));
        }
        glEnd();
    }
    
    _lock.unlock();
}

void WandererChunk::maintain(Renderer* renderer)
{
    _lock.lock();
    
    if (_dirty)
    {
        _dirty = false;
        
        // Compute heightmap
        for (int j = 0; j <= _nbsubchunks; j++)
        {
            for (int i = 0; i <= _nbsubchunks; i++)
            {
                _heightmap[j * (_nbsubchunks + 1) + i] = renderer->getTerrainHeight(renderer, _startx + _subchunksize * (double)i, _startz + _subchunksize * (double)j);
            }
        }
        
        // Compute texture
        int texture_size = 4;
        double step_size = _chunksize / (double)(texture_size - 1);
        for (int j = 0; j < texture_size; j++)
        {
            for (int i = 0; i < texture_size; i++)
            {
                Vector3 location = {_startx + step_size * (double)i, 0.0, _startz + step_size * (double)j};
                Color color = renderer->applyTextures(renderer, location, step_size);
                _texture->setPixel(i, j, colorTo32BitRGBA(&color));
            }
        }
        _need_texture_upload = true;
    }
    
    _lock.unlock();
}
