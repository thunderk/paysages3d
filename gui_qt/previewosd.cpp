#include "previewosd.h"

#include <QHash>
#include <QPainter>
#include <math.h>

static QHash<QString, PreviewOsd*> _instances;

/*************** PreviewOsdItem ***************/
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
    int w2 = width() / 2;
    int h2 = height() / 2;
    
    _xlocation = camera->location.x;
    _ylocation = camera->location.z;
    
    QPainter painter(this);
    painter.setPen(QPen(Qt::red, 2));
    painter.setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing, true);
    painter.drawLine(w2, h2, w2 + w2 * cos(camera->yaw - M_PI_4), h2 - h2 * sin(camera->yaw - M_PI_4));
    painter.drawLine(w2, h2, w2 + w2 * cos(camera->yaw + M_PI_4), h2 - h2 * sin(camera->yaw + M_PI_4));
}

/*************** PreviewOsd ***************/
PreviewOsd::PreviewOsd()
{
}

PreviewOsd::~PreviewOsd()
{
    for (int i = 0; i < _items.size(); i++)
    {
        delete _items[i];
    }
}

PreviewOsd* PreviewOsd::getInstance(QString name)
{
    if (_instances.contains(name))
    {
        return _instances[name];
    }
    else
    {
        PreviewOsd* instance = new PreviewOsd();
        _instances.insert(name, instance);
        return instance;
    }
}

void PreviewOsd::clearItems()
{
    for (int i = 0; i < _items.size(); i++)
    {
        delete _items[i];
    }
    _items.clear();
}
    
PreviewOsdItem* PreviewOsd::newItem(int width, int height)
{
    PreviewOsdItem* item = new PreviewOsdItem(width, height);
    _items.append(item);
    return item;
}

PreviewOsdItem* PreviewOsd::newItem(QImage image)
{
    PreviewOsdItem* item = newItem(image.width(), image.height());
    QPainter painter(item);
    painter.drawImage(0, 0, image);
    return item;
}

void PreviewOsd::apply(QImage* mask, double xoffset, double yoffset, double scaling)
{
    QPainter painter(mask);
    
    for (int i = 0; i < _items.size(); i++)
    {
        PreviewOsdItem* item = _items[i];
        int x = (int)(mask->width() / 2 - (xoffset - item->xlocation()) / scaling - item->width() / 2);
        int y = (int)(mask->height() / 2 - (yoffset - item->ylocation()) / scaling - item->height() / 2);
        painter.drawImage(x, y, *item);
    }
}
