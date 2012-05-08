#ifndef _PAYSAGES_QT_WANDERERCHUNK_H_
#define _PAYSAGES_QT_WANDERERCHUNK_H_

#include <QMutex>
#include <QImage>
#include <QGLWidget>
#include "../lib_paysages/renderer.h"

class WandererChunk
{
public:
    WandererChunk(Renderer* renderer, double x, double z, double size);
    ~WandererChunk();
    
    bool maintain(Vector3 camera_location);
    void render(QGLWidget* widget);
    
    Vector3 getCenter();
    
    double _ideal_priority;
    
private:
    QMutex _lock_data;
    Renderer* _renderer;
    
    double _startx;
    double _startz;
    double _size;
    
    int _ideal_tessellation;
    
    double* _tessellation;
    int _tessellation_max_size;
    int _tessellation_current_size;
    double _tessellation_step;
    
    QImage* _texture;
    GLuint _texture_id;
    bool _texture_changed;
    int _texture_max_size;
    int _texture_current_size;
};

#endif
