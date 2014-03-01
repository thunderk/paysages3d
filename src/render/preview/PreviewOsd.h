#ifndef PREVIEWOSD_H
#define PREVIEWOSD_H

#include "preview_global.h"

#include <QVector>
class QImage;

namespace paysages {
namespace preview {

class PREVIEWSHARED_EXPORT PreviewOsd
{
public:
    PreviewOsd();
    ~PreviewOsd();

    static PreviewOsd* getInstance(QString name);

    void clearItems();
    PreviewOsdItem* newItem(int width, int height);
    PreviewOsdItem* newItem(const QImage& image);
    void apply(QImage* mask, double xoffset, double yoffset, double scaling);
    QString getToolTip(double x, double y, double scaling);

private:
    QVector<PreviewOsdItem*> _items;
};

}
}

#endif
