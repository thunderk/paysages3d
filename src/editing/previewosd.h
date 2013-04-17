#ifndef _PAYSAGES_QT_PREVIEWOSD_H_
#define _PAYSAGES_QT_PREVIEWOSD_H_

#include <QImage>
#include "rendering/camera.h"

class PreviewOsdItem:public QImage
{
public:
    PreviewOsdItem(int width, int height);
    
    void setLocation(double x, double y);
    inline double xlocation() {return _xlocation;};
    inline double ylocation() {return _ylocation;};
    
    void setToolTip(QString text);
    QString getToolTip(double x, double y, double scaling);
    
    void drawCamera(CameraDefinition* camera);
    
private:
    double _xlocation;
    double _ylocation;
    QString _tooltip;
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
    QString getToolTip(double x, double y, double scaling);
    
private:
    QVector<PreviewOsdItem*> _items;
};

#endif
