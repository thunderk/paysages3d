#include "previewosd.h"

#include <QHash>
#include <QPainter>

static QHash<QString, PreviewOsd*> _instances;

/*************** PreviewOsdItem ***************/
PreviewOsdItem::PreviewOsdItem(int width, int height) : QImage(width, height, QImage::Format_ARGB32)
{
    _xlocation = 0.0;
    _ylocation = 0.0;
}

void PreviewOsdItem::setLocation(double x, double y)
{
    _xlocation = x;
    _ylocation = y;
}

/*************** PreviewOsd ***************/
PreviewOsd::PreviewOsd()
{
    newItem(50, 50)->fill(0x88888888);
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
