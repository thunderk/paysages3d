#ifndef _PAYSAGES_QT_DIALOGEXPLORER_H_
#define _PAYSAGES_QT_DIALOGEXPLORER_H_

#include "desktop_global.h"

#include <QDialog>

class QSlider;

class DialogExplorer : public QDialog
{
    Q_OBJECT
public:
    explicit DialogExplorer(QWidget *parent, CameraDefinition* camera, bool camera_validable=false, Scenery* scenery=0);
    ~DialogExplorer();

protected slots:
    void validateCamera();
    void resetCamera();
    void fovChanged(int value);

private:
    WidgetExplorer* _wanderer;
    QSlider* _fov;
};

#endif
