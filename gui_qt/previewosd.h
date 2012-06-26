#ifndef _PAYSAGES_QT_PREVIEWOSD_H_
#define _PAYSAGES_QT_PREVIEWOSD_H_

#include <QImage>
#include "../lib_paysages/camera.h"

class PreviewOsdItem:public QImage
{
public:
    PreviewOsdItem(int width, int height);
    
    void setLocation(double x, double y);
    inline double xlocation() {return _xlocation;};
    inline double ylocation() {return _ylocation;};
    
    void drawCamera(CameraDefinition* camera);
    
private:
    double _xlocation;
    double _ylocation;
};

class PreviewOsd
{
public:
    PreviewOsd();
    ~PreviewOsd();
    
    static PreviewOsd* getInstance(QString name);
    
    void clearItems();
    PreviewOsdItem* newItem(int width, int height);
    PreviewOsdItem* newItem(QImage image);
    void apply(QImage* mask, double xoffset, double yoffset, double scaling);
    
private:
    QVector<PreviewOsdItem*> _items;
};

#endif
