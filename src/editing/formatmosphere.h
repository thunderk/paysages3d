#ifndef _PAYSAGES_QT_FORMATMOSPHERE_H_
#define _PAYSAGES_QT_FORMATMOSPHERE_H_

#include "editing_global.h"

#include "baseform.h"
class QWidget;

class FormAtmosphere : public BaseForm
{
    Q_OBJECT

public:
    explicit FormAtmosphere(QWidget *parent = 0);
    virtual ~FormAtmosphere();

public slots:
    virtual void revertConfig();
    virtual void applyConfig();

protected slots:
    virtual void configChangeEvent();
    virtual void autoPresetSelected(int preset);

private:
    BasePreview* previewEast;
    Base2dPreviewRenderer* previewEastRenderer;
    BasePreview* previewWest;
    Base2dPreviewRenderer* previewWestRenderer;
};

#endif
