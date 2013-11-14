#include "PreviewOsdItem.h"

#include <QPainter>
#include "Vector3.h"
#include "CameraDefinition.h"

PreviewOsdItem::PreviewOsdItem(int width, int height) : QImage(width, height, QImage::Format_ARGB32)
{
    _xlocation = 0.0;
    _ylocation = 0.0;
    fill(0x00000000);
}

void PreviewOsdItem::setLocation(double x, double y)
{
    _xlocation = x;
    _ylocation = y;
}

void PreviewOsdItem::drawCamera(CameraDefinition* camera)
{
    Vector3 camera_location = camera->getLocation();
    VectorSpherical camera_direction = camera->getDirectionSpherical();
    int w2 = width() / 2;
    int h2 = height() / 2;

    _xlocation = camera_location.x;
    _ylocation = camera_location.z;

    QPainter painter(this);
    painter.setPen(QPen(Qt::red, 2));
    painter.setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing, true);
    painter.drawLine(w2, h2, w2 + w2 * cos(camera_direction.phi - M_PI_4), h2 - h2 * sin(camera_direction.phi - M_PI_4));
    painter.drawLine(w2, h2, w2 + w2 * cos(camera_direction.phi + M_PI_4), h2 - h2 * sin(camera_direction.phi + M_PI_4));
}

void PreviewOsdItem::setToolTip(QString text)
{
    _tooltip = text;
}

QString PreviewOsdItem::getToolTip(double x, double y, double scaling)
{
    if (_tooltip.isEmpty() or (x > _xlocation - (width() / 2) * scaling and x < _xlocation + (width() / 2) * scaling and y > _ylocation - (height() / 2) * scaling and y < _ylocation + (height() / 2) * scaling))
    {
        return _tooltip;
    }
    else
    {
        return QString();
    }
}
