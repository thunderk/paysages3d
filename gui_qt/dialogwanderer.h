#ifndef _PAYSAGES_QT_DIALOGWANDERER_H_
#define _PAYSAGES_QT_DIALOGWANDERER_H_

#include <QDialog>
#include "widgetwanderer.h"
#include "../lib_paysages/camera.h"

class DialogWanderer : public QDialog
{
    Q_OBJECT
public:
    explicit DialogWanderer(QWidget *parent, CameraDefinition* camera, bool camera_validable);
    ~DialogWanderer();

protected slots:
    void validateCamera();

private:
    WidgetWanderer* _wanderer;
};

#endif
