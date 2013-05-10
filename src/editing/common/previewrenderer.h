#ifndef PREVIEWRENDERER_H
#define PREVIEWRENDERER_H

#include <QObject>
#include "rendering/tools/color.h"
#include "rendering/renderer.h"

class BasePreview;

class PreviewRenderer : public QObject
{
    Q_OBJECT

public:
    explicit PreviewRenderer(QObject *parent = 0);
    ~PreviewRenderer();

    virtual void bindEvent(BasePreview* preview);
    virtual void updateEvent();
    virtual Color getColor2D(double x, double y, double scaling);

signals:
    
public slots:
    
protected:
    Renderer* renderer;
};

#endif // PREVIEWRENDERER_H
