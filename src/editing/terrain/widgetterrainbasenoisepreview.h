#ifndef _PAYSAGES_EDITING_TERRAIN_WIDGETTERRAINBASENOISEPREVIEW_H_
#define _PAYSAGES_EDITING_TERRAIN_WIDGETTERRAINBASENOISEPREVIEW_H_

#include <QWidget>
#include "rendering/noise.h"

class WidgetTerrainBaseNoisePreview : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetTerrainBaseNoisePreview(QWidget* parent = 0);

    void setNoise(NoiseGenerator* noise);

protected:
    virtual void paintEvent(QPaintEvent* event);

signals:
    
public slots:

private:
    NoiseGenerator* _noise;
};

#endif
