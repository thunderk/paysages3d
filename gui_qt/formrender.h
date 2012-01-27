#ifndef _PAYSAGES_QT_FORMRENDER_H_
#define _PAYSAGES_QT_FORMRENDER_H_

#include "baseform.h"
#include "../lib_paysages/camera.h"

class FormRender : public BaseForm
{
    Q_OBJECT

public:
    explicit FormRender(QWidget *parent = 0);

public slots:
    virtual void revertConfig();
    virtual void applyConfig();

protected slots:
    virtual void configChangeEvent();

private slots:
    void startRender();
    void showRender();
    void saveRender();

private:
    int _quality;
    int _width;
    int _height;
    CameraDefinition _camera;
};

#endif // _PAYSAGES_QT_FORMRENDER_H_
