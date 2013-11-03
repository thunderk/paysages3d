#ifndef _PAYSAGES_EDITING_TERRAIN_WIDGETTERRAINBASENOISEPREVIEW_H_
#define _PAYSAGES_EDITING_TERRAIN_WIDGETTERRAINBASENOISEPREVIEW_H_

#include "editing_global.h"
#include "editing/common/DrawingWidget.h"

class QPainter;

namespace paysages {
namespace basics {
    class NoiseGenerator;
}
}

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
