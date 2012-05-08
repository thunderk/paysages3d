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
    
    void maintain(Renderer* renderer);
    void render(QGLWidget* widget);
    
private:
    QMutex _lock;
    double _startx;
    double _startz;
    bool _dirty;
    double _chunksize;
    double _subchunksize;
    int _nbsubchunks;
    QImage* _texture;
    GLuint _texture_id;
    bool _need_texture_upload;
    double* _heightmap;
};

#endif
