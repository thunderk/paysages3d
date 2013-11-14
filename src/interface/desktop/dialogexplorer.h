#ifndef _PAYSAGES_QT_DIALOGEXPLORER_H_
#define _PAYSAGES_QT_DIALOGEXPLORER_H_

#include "desktop_global.h"

#include <QDialog>

class DialogExplorer : public QDialog
{
    Q_OBJECT
public:
    explicit DialogExplorer(QWidget *parent, CameraDefinition* camera, bool camera_validable=false, SoftwareRenderer* renderer=0);
    ~DialogExplorer();

protected slots:
    void validateCamera();

private:
    WidgetExplorer* _wanderer;
};

#endif
