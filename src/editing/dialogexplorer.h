#ifndef _PAYSAGES_QT_DIALOGEXPLORER_H_
#define _PAYSAGES_QT_DIALOGEXPLORER_H_

#include "editing_global.h"

#include <QDialog>

class Renderer;

class DialogExplorer : public QDialog
{
    Q_OBJECT
public:
    explicit DialogExplorer(QWidget *parent, CameraDefinition* camera, bool camera_validable=false, Renderer* renderer=0);
    ~DialogExplorer();

protected slots:
    void validateCamera();

private:
    WidgetExplorer* _wanderer;
};

#endif
