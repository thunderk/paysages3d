#ifndef OPENGLTERRAINCHUNK_H
#define OPENGLTERRAINCHUNK_H

#include "opengl_global.h"

class QImage;

namespace paysages {
namespace opengl {

class OPENGLSHARED_EXPORT OpenGLTerrainChunk {
  public:
    OpenGLTerrainChunk(OpenGLRenderer *renderer, double x, double z, double size, int nbchunks);
    ~OpenGLTerrainChunk();

    bool maintain();
    void updatePriority(CameraDefinition *camera);
    void render(OpenGLShaderProgram *program);

    void askReset(bool topology = true, bool texture = true);
    void askInterrupt();
    void askResume();

    inline int getVerticesLevel() const {
        return vertices_level;
    }
    inline const OpenGLVertexArray *getVertices() const {
        return vertices;
    }

    /**
     * Fill *vertices* with a quick initial set of vertices, that can be augmented later using *augmentVertices*.
     */
    void setFirstStepVertices();

    /**
     * Improve the level of detail of tessellated vertices in *vertices*.
     *
     * This will double the existing resolution.
     */
    void augmentVertices();

    /**
     * Update *vertices* using *source*.
     */
    void updateVertices(const OpenGLVertexArray &source, int vertice_level);

    /**
     * Set a square (two triangles) in *vertices_next*.
     */
    void fillVerticesFromSquare(OpenGLVertexArray *array, int index_offset, double x, double z, double size);

    double priority;

  private:
    Vector3 getCenter();

    double _startx;
    double _startz;
    double _size;
    double _overall_step;

    OpenGLVertexArray *vertices;
    int vertices_level;

    Mutex *_lock_data;

    OpenGLRenderer *_renderer;
    OpenGLSharedState *glstate;

    bool _reset_topology;
    bool _reset_texture;
    bool interrupt;

    QImage *_texture;
    bool _texture_changed;
    int _texture_current_size;
    int _texture_wanted_size;
    int _texture_max_size;

    // LOD control
    double distance_to_camera;
};
}
}

#endif // OPENGLTERRAINCHUNK_H
