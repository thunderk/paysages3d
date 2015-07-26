#ifndef EXPLORERCHUNKTERRAIN_H
#define EXPLORERCHUNKTERRAIN_H

#include "opengl_global.h"

#include <QMutex>
class QImage;
class QOpenGLShaderProgram;

namespace paysages {
namespace opengl {

class OPENGLSHARED_EXPORT ExplorerChunkTerrain
{
public:
    typedef struct
    {
        float location[3];
        float uv[2];
    } TerrainVertex;

public:
    ExplorerChunkTerrain(OpenGLRenderer* renderer, double x, double z, double size, int nbchunks, double water_height);
    ~ExplorerChunkTerrain();

    bool maintain();
    void updatePriority(CameraDefinition* camera);
    void render(QOpenGLShaderProgram* program, OpenGLFunctions* functions);

    void askReset(bool topology = true, bool texture = true);
    void askInterrupt();

    Color getTextureColor(double x, double y);

    double priority;

private:
    Vector3 getCenter();

    double _startx;
    double _startz;
    double _size;
    double _overall_step;

    double _water_height;

    int tessellation_count;
    VertexArray<TerrainVertex> *tessellated;
    int _tessellation_max_size;
    int _tessellation_current_size;
    double _tessellation_step;

    QMutex _lock_data;

    OpenGLRenderer* _renderer;

    bool _reset_topology;
    bool _reset_texture;
    bool interrupt;

    QImage* _texture;
    unsigned int texture_id;
    bool _texture_changed;
    int _texture_current_size;
    int _texture_wanted_size;
    int _texture_max_size;

    // LOD control
    double distance_to_camera;
    bool overwater;
};

}
}

#endif // EXPLORERCHUNKTERRAIN_H
