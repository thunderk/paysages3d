#ifndef _PAYSAGES_QT_TOOLS_H_
#define _PAYSAGES_QT_TOOLS_H_

#include <QColor>
#include <QDialog>
#include "DataFile.h"

#include "Color.h"

static inline QColor colorToQColor(Color color)
{
    color.normalize();
    return QColor(color.r * 255.0, color.g * 255.0, color.b * 255.0, color.a * 255.0);
}

static inline QString getDataPath(QString path)
{
    return QString::fromStdString(DataFile::findFile(path.toStdString()));
}

QString getHumanMemory(qint64 memused);

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
