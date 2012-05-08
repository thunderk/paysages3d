#ifndef _PAYSAGES_QT_WANDERERCHUNK_H_
#define _PAYSAGES_QT_WANDERERCHUNK_H_

#include <QMutex>
#include <QImage>
#include <QGLWidget>
#include "../lib_paysages/renderer.h"

class WandererChunk
{
public:
    WandererChunk(double x, double z, double size);
    ~WandererChunk();
    
    bool maintain(Renderer* renderer);
    void render(QGLWidget* widget);
    
    Vector3 getCenter();
    
private:
    QMutex _lock;
    QMutex _lock_dirty;
    double _startx;
    double _startz;
    int _dirty;
    double _chunksize;
    double _subchunksize;
    int _nbsubchunks;
    QImage* _texture;
    GLuint _texture_id;
    bool _need_texture_upload;
    double* _heightmap;
};

#endif
