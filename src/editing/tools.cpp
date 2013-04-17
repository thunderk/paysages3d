#include "tools.h"

#include <QEvent>
#include "basepreview.h"

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
