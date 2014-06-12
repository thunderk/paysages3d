#ifndef _PAYSAGES_QT_FORMRENDER_H_
#define _PAYSAGES_QT_FORMRENDER_H_

#include "desktop_global.h"

#include "baseform.h"

#include "RenderArea.h"

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
    void startRender();
    void showRender();

protected slots:
    virtual void configChangeEvent();

private:
    RenderArea::RenderParams _params;
    CameraDefinition* _camera;
    SoftwareCanvasRenderer* _renderer;
    bool _renderer_inited;
    BasePreview* _preview_landscape;
    Base2dPreviewRenderer* _preview_landscape_renderer;
};

#endif
