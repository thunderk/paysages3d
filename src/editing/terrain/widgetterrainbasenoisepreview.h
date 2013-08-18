#ifndef _PAYSAGES_EDITING_TERRAIN_WIDGETTERRAINBASENOISEPREVIEW_H_
#define _PAYSAGES_EDITING_TERRAIN_WIDGETTERRAINBASENOISEPREVIEW_H_

#include "editing/common/DrawingWidget.h"

#include "rendering/noise.h"

class QPainter;

class WidgetTerrainBaseNoisePreview : public DrawingWidget
{
    Q_OBJECT
public:
    explicit WidgetTerrainBaseNoisePreview(QWidget* parent = 0);

    void setNoise(NoiseGenerator* noise);

protected:
    virtual void doDrawing(QPainter* painter);

private:
    NoiseGenerator* _noise;
};

#endif
