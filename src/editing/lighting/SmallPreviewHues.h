#ifndef SMALLPREVIEWHUES_H
#define	SMALLPREVIEWHUES_H

#include "editing/common/DrawingWidget.h"

#include "rendering/tools/color.h"

class SmallPreviewHues: public DrawingWidget
{
    Q_OBJECT
public:
    SmallPreviewHues(QWidget* parent);
protected:
    virtual void doDrawing(QPainter* painter);
};

#endif	/* SMALLPREVIEWHUES_H */

