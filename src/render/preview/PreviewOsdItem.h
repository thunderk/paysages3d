#ifndef PREVIEWOSDITEM_H
#define PREVIEWOSDITEM_H

#include "preview_global.h"

#include <QImage>

class CameraDefinition;

namespace paysages {
namespace preview {

class PreviewOsdItem:public QImage
{
public:
    PreviewOsdItem(int width, int height);

    void setLocation(double x, double y);
    inline double xlocation() {return _xlocation;}
    inline double ylocation() {return _ylocation;}

    void setToolTip(QString text);
    QString getToolTip(double x, double y, double scaling);

    void drawCamera(CameraDefinition* camera);

private:
    double _xlocation;
    double _ylocation;
    QString _tooltip;
};

}
}

#endif // PREVIEWOSDITEM_H
