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

    void askReset();
    void setMaxTextureSize(int size);

    bool onMaintainEvent();
    double getDisplayedSizeHint(CameraDefinition* camera);
    Color getTextureColor(double x, double y);

    double priority;

private:
    Vector3 getCenter();

    double _startx;
    double _startz;
    double _size;
    double _overall_step;

    double _distance_to_camera;
    double _water_height;
    bool _overwater;

    int tessellation_count;
    VertexArray<TerrainVertex> *tessellated;
    int _tessellation_max_size;
    int _tessellation_current_size;
    double _tessellation_step;

    QMutex _lock_data;

    OpenGLRenderer* _renderer;
    ColorProfile* _color_profile;

    bool _reset_needed;

    QImage* _texture;
    unsigned int texture_id;
    bool _texture_changed;
    int _texture_current_size;
    int _texture_max_size;
};

}
}

#endif // EXPLORERCHUNKTERRAIN_H
