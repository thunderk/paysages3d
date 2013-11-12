#include "PreviewOsd.h"

#include <QImage>
#include <QHash>
#include <QPainter>
#include "PreviewOsdItem.h"

static QHash<QString, PreviewOsd*> _instances;

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

PreviewOsdItem* PreviewOsd::newItem(const QImage &image)
{
    PreviewOsdItem* item = newItem(image.width(), image.height());
    QPainter painter(item);
    painter.drawImage(0, 0, image);
    return item;
}

void PreviewOsd::apply(QImage *mask, double xoffset, double yoffset, double scaling)
{
    QPainter painter(mask);

    for (int i = 0; i < _items.size(); i++)
    {
        PreviewOsdItem* item = _items[i];
        int x = (int) (mask->width() / 2 - (xoffset - item->xlocation()) / scaling - item->width() / 2);
        int y = (int) (mask->height() / 2 - (yoffset - item->ylocation()) / scaling - item->height() / 2);
        painter.drawImage(x, y, *item);
    }
}

QString PreviewOsd::getToolTip(double x, double y, double scaling)
{
    for (int i = 0; i < _items.size(); i++)
    {
        PreviewOsdItem* item = _items[i];
        QString tooltip = item->getToolTip(x, y, scaling);
        if (not tooltip.isEmpty())
        {
            return tooltip;
        }
    }
    return QString();
}
