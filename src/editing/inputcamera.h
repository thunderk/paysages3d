#ifndef _PAYSAGES_QT_INPUTCAMERA_H_
#define _PAYSAGES_QT_INPUTCAMERA_H_

#include "editing_global.h"

#include <QWidget>
#include "baseinput.h"

class InputCamera:public BaseInput
{
    Q_OBJECT

public:
    InputCamera(QWidget* form, QString label, CameraDefinition* value);

public slots:
    virtual void updatePreview();
    virtual void applyValue();
    virtual void revert();

private slots:
    void editCamera();

private:
    CameraDefinition* _value;
};

#endif
