#include "widgetterrainbasenoisepreview.h"

#include <QPainter>
#include <QPaintEvent>
#include "tools.h"

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
    painter->setBrush(Qt::SolidPattern);
    painter->drawRect(rect());

    int height = this->height();

    if (_noise)
    {
        //QRect boundaries = event->region().boundingRect();
        QRect boundaries = rect(); // TODO Limit to updated region
        double value, factor;
        double minvalue, maxvalue;

        noiseGetRange(_noise, &minvalue, &maxvalue);
        factor = ((double)height) / (maxvalue - minvalue);

        for (int x = boundaries.left(); x <= boundaries.right(); x++)
        {
            value = noiseGet1DTotal(_noise, 100.0 * ((double)x) / factor);

            painter->setPen(Qt::white);
            painter->drawLine(x, height - 1 - (value - minvalue) * factor, x, height - 1);
        }
    }
}
