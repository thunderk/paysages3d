#include "OpenGLTerrainChunk.h"

#include <cmath>
#include <cassert>
#include <QImage>
#include "OpenGLShaderProgram.h"
#include "OpenGLVertexArray.h"
#include "OpenGLSharedState.h"
#include "ColorProfile.h"
#include "CameraDefinition.h"
#include "OpenGLRenderer.h"
#include "TerrainRenderer.h"
#include "TexturesRenderer.h"
#include "Scenery.h"
#include "TerrainDefinition.h"
#include "Texture2D.h"
#include "Mutex.h"
#include "Logs.h"

OpenGLTerrainChunk::OpenGLTerrainChunk(OpenGLRenderer *renderer, double x, double z, double size, int nbchunks)
    : _renderer(renderer) {
    priority = 0.0;
    _reset_topology = false;
    _reset_texture = false;

    interrupt = false;

    _lock_data = new Mutex();

    _texture = new QImage(1, 1, QImage::Format_RGBA8888);
    _texture_changed = false;
    _texture_current_size = 0;
    _texture_wanted_size = 0;
    _texture_max_size = 256;

    _startx = x;
    _startz = z;
    _size = size;
    _overall_step = size * to_double(nbchunks);

    distance_to_camera = 0.0;

    vertices = new OpenGLVertexArray(true);
    vertices_level = 0;
    glstate = new OpenGLSharedState();
    Texture2D empty(1, 1);
    glstate->set("groundTexture", &empty);
}

OpenGLTerrainChunk::~OpenGLTerrainChunk() {
    _lock_data->acquire();

    delete _texture;
    delete vertices;
    delete glstate;

    _lock_data->release();

    delete _lock_data;
}

bool OpenGLTerrainChunk::maintain() {
    bool subchanged;

    _lock_data->acquire();
    if (_reset_topology) {
        _reset_topology = false;
        vertices_level = 0;
    }
    if (_reset_texture) {
        _reset_texture = false;
        _texture_current_size = 0;
    }

    _lock_data->release();

    // Improve heightmap resolution
    if (vertices_level < 8) {
        if (vertices_level) {
            augmentVertices();
        } else {
            setFirstStepVertices();
        }
        return true;
    } else if (vertices_level < 64) {
        augmentVertices();
        subchanged = true;
    }

    // Improve texture resolution
    if (_texture_current_size < _texture_wanted_size) {
        int new_texture_size = _texture_current_size ? _texture_current_size * 2 : 1;
        QImage *new_image = new QImage(_texture->scaled(new_texture_size + 1, new_texture_size + 1,
                                                        Qt::IgnoreAspectRatio, Qt::FastTransformation));
        double factor = _size / to_double(new_texture_size);
        for (int j = 0; j <= new_texture_size; j++) {
            for (int i = 0; i <= new_texture_size; i++) {
                if (_texture_current_size <= 1 || i % 2 != 0 || j % 2 != 0) {
                    double x = _startx + factor * to_double(i);
                    double z = _startz + factor * to_double(j);
                    Color color = _renderer->getTerrainRenderer()->getFinalColor(x, z, 0.001);
                    new_image->setPixel(i, j, Color(color.r * 0.2, color.g * 0.2, color.b * 0.2).normalized().to32BitRGBA());
                }
            }

            if (interrupt or _reset_texture) {
                return false;
            }
        }

        _lock_data->acquire();
        delete _texture;
        _texture = new_image;
        _texture_current_size = new_texture_size;
        _texture_changed = true;
        _lock_data->release();

        return true;
    } else {
        return subchanged;
    }
}

void OpenGLTerrainChunk::updatePriority(CameraDefinition *camera) {
    Vector3 camera_location = camera->getLocation();

    // Handle position
    _lock_data->acquire();
    if (camera_location.x > _startx + _overall_step * 0.5) {
        _startx += _overall_step;
        askReset();
    }
    if (camera_location.z > _startz + _overall_step * 0.5) {
        _startz += _overall_step;
        askReset();
    }
    if (camera_location.x < _startx - _overall_step * 0.5) {
        _startx -= _overall_step;
        askReset();
    }
    if (camera_location.z < _startz - _overall_step * 0.5) {
        _startz -= _overall_step;
        askReset();
    }
    distance_to_camera = getCenter().sub(camera_location).getNorm();
    _lock_data->release();

    // Update wanted LOD
    if (distance_to_camera < 100.0) {
        _texture_wanted_size = _texture_max_size;
    } else if (distance_to_camera < 200.0) {
        _texture_wanted_size = _texture_max_size / 4;
    } else if (distance_to_camera < 400.0) {
        _texture_wanted_size = _texture_max_size / 8;
    } else {
        _texture_wanted_size = _texture_max_size / 16;
    }

    // Update priority
    if (_reset_topology || _reset_texture || (_texture_max_size > 1 && _texture_current_size <= 1) ||
        vertices_level < 8) {
        priority = 1000.0 - to_double(vertices_level);
    } else if (_texture_current_size == _texture_wanted_size) {
        priority = -1000.0;
    } else {
        priority = _texture_wanted_size / _texture_current_size;
    }
}

void OpenGLTerrainChunk::render(OpenGLShaderProgram *program) {
    // Put texture in place
    _lock_data->acquire();
    if (_texture_changed) {
        _texture_changed = false;
        glstate->set("groundTexture", *_texture);
        /*glstate->set("groundTexture", _texture->scaled(_texture_current_size, _texture_current_size,
                                                       Qt::IgnoreAspectRatio, Qt::SmoothTransformation));*/
    }
    _lock_data->release();

    // FIXME Should update 'vertices' inside lock
    program->draw(vertices, glstate);
}

void OpenGLTerrainChunk::askReset(bool topology, bool texture) {
    _reset_topology = _reset_topology or topology;
    _reset_texture = _reset_texture or texture;
}

void OpenGLTerrainChunk::askInterrupt() {
    interrupt = true;
}

void OpenGLTerrainChunk::askResume() {
    interrupt = false;
}

void OpenGLTerrainChunk::destroy(OpenGLFunctions *functions) {
    vertices->destroy(functions);
    glstate->destroy(functions);
}

void OpenGLTerrainChunk::setFirstStepVertices() {
    OpenGLVertexArray next(true);
    next.setVertexCount(6);
    fillVerticesFromSquare(&next, 0, _startx, _startz, _size);
    updateVertices(next, 1);
}

void OpenGLTerrainChunk::augmentVertices() {
    OpenGLVertexArray next(true);
    next.setVertexCount(vertices->getVertexCount() * 4);
    int next_vertices_level = vertices_level * 2;

    // TODO Re-use existing vertices from previous level when possible
    double quad_size = _size / to_double(next_vertices_level);
    for (int iz = 0; iz < next_vertices_level; iz++) {
        if (interrupt or _reset_topology) {
            return;
        }
        for (int ix = 0; ix < next_vertices_level; ix++) {
            fillVerticesFromSquare(&next, (iz * next_vertices_level + ix) * 6, _startx + quad_size * to_double(ix),
                                   _startz + quad_size * to_double(iz), quad_size);
        }
    }

    updateVertices(next, next_vertices_level);
}

void OpenGLTerrainChunk::updateVertices(const OpenGLVertexArray &source, int vertice_level) {
    assert(source.getVertexCount() == vertice_level * vertice_level * 6);

    _lock_data->acquire();

    source.copyTo(vertices);
    vertices_level = vertice_level;

    _lock_data->release();
}

void OpenGLTerrainChunk::fillVerticesFromSquare(OpenGLVertexArray *array, int index_offset, double x, double z,
                                                double size) {
    Vector3 c1(x, _renderer->getTerrainRenderer()->getHeight(x, z, true, false), z);
    Vector3 c2(x, _renderer->getTerrainRenderer()->getHeight(x, z + size, true, false), z + size);
    Vector3 c3(x + size, _renderer->getTerrainRenderer()->getHeight(x + size, z + size, true, false), z + size);
    Vector3 c4(x + size, _renderer->getTerrainRenderer()->getHeight(x + size, z, true, false), z);

    double u = (x - _startx) / _size;
    double v = (z - _startz) / _size;
    double dt = size / _size;

    array->set(index_offset, c1, u, v);
    array->set(index_offset + 1, c2, u, v + dt);
    array->set(index_offset + 2, c4, u + dt, v);

    array->set(index_offset + 3, c3, u + dt, v + dt);
    array->set(index_offset + 4, c4, u + dt, v);
    array->set(index_offset + 5, c2, u, v + dt);
}

Vector3 OpenGLTerrainChunk::getCenter() {
    Vector3 result;

    result.x = _startx + _size / 2.0;
    result.y = 0.0;
    result.z = _startz + _size / 2.0;

    return result;
}
