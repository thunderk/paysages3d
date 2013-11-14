#include "widgetterrainbasenoisepreview.h"

#include <QPainter>
#include <QPaintEvent>
#include "tools.h"
#include "NoiseGenerator.h"

WidgetTerrainBaseNoisePreview::WidgetTerrainBaseNoisePreview(QWidget* parent) :
    DrawingWidget(parent)
{
    _noise = NULL;
}

void WidgetTerrainBaseNoisePreview::setNoise(NoiseGenerator* noise)
{
    _noise = noise;
    update();
}

void WidgetTerrainBaseNoisePreview::doDrawing(QPainter* painter)
{
    painter->fillRect(rect(), QColor(20, 40, 80));
    painter->setPen(QColor(220, 210, 180));

    int height = this->height();

    if (_noise)
    {
        //QRect boundaries = event->region().boundingRect();
        QRect boundaries = rect(); // TODO Limit to updated region
        double value, factor;
        double minvalue, maxvalue;

        _noise->getRange(&minvalue, &maxvalue);
        factor = ((double)height) / (maxvalue - minvalue);

        for (int x = boundaries.left(); x <= boundaries.right(); x++)
        {
            value = _noise->get1DTotal(100.0 * ((double)x) / factor);

            painter->drawLine(x, height - 1 - (value - minvalue) * factor, x, height - 1);
        }
    }
}
