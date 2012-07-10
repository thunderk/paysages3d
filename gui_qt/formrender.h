#ifndef _PAYSAGES_QT_FORMRENDER_H_
#define _PAYSAGES_QT_FORMRENDER_H_

#include "baseform.h"
#include "../lib_paysages/camera.h"
#include "../lib_paysages/renderer.h"
#include "../lib_paysages/render.h"

class FormRender : public BaseForm
{
    Q_OBJECT

public:
    explicit FormRender(QWidget *parent = 0);
    ~FormRender();

    virtual void savePack(PackStream* stream);
    virtual void loadPack(PackStream* stream);

public slots:
    virtual void revertConfig();
    virtual void applyConfig();
    void startQuickRender();

protected slots:
    virtual void configChangeEvent();

private slots:
    void startRender();
    void showRender();
    void saveRender();

private:
    RenderParams _params;
    CameraDefinition _camera;
    Renderer _renderer;
    bool _renderer_inited;
    BasePreview* _preview_landscape;
};

#endif
