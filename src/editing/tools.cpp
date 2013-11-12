#include "tools.h"

#include <QEvent>
#include "BasePreview.h"

DialogWithPreview::DialogWithPreview(QWidget* parent) : QDialog(parent)
{
}

bool DialogWithPreview::event(QEvent* event)
{
    if (event->type() == QEvent::WindowActivate)
    {
        BasePreview::reviveAll();
    }
    
    return QDialog::event(event);
}

QString getHumanMemory(qint64 memused)
{
    if (memused >= 1024)
    {
        memused /= 1024;
        if (memused >= 1024)
        {
            memused /= 1024;
            if (memused >= 1024)
            {
                memused /= 1024;
                return QObject::tr("%1 GB").arg(memused);
            }
            else
            {
                return QObject::tr("%1 MB").arg(memused);
            }
        }
        else
        {
            return QObject::tr("%1 kB").arg(memused);
        }
    }
    else
    {
        return QObject::tr("%1 B").arg(memused);
    }
}
