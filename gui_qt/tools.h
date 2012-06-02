#ifndef _PAYSAGES_QT_TOOLS_H_
#define _PAYSAGES_QT_TOOLS_H_

#include <QColor>
#include <QDialog>

#include "../lib_paysages/shared/types.h"
#include "../lib_paysages/color.h"

static inline QColor colorToQColor(Color color)
{
    colorNormalize(&color);
    return QColor(color.r * 255.0, color.g * 255.0, color.b * 255.0, color.a * 255.0);
}

class DialogWithPreview:public QDialog
{
    Q_OBJECT
public:
    DialogWithPreview(QWidget* parent);
    
protected:
    virtual bool event(QEvent* event);
};

#ifndef NDEBUG
#include <QDebug>
static inline void logDebug(QString string)
{
    qDebug() << string;
}
#else
#define logDebug(_x_) ;
#endif

#endif
