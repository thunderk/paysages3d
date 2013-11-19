#ifndef _PAYSAGES_QT_FORMCLOUDS_H_
#define _PAYSAGES_QT_FORMCLOUDS_H_

#include "desktop_global.h"

#include "baseformlayer.h"
#include "clouds/public.h"
class QWidget;

class FormClouds : public BaseFormLayer
{
    Q_OBJECT

public:
    explicit FormClouds(QWidget *parent = 0);
    ~FormClouds();

public slots:
    virtual void revertConfig();
    virtual void applyConfig();

protected:
    virtual void layerReadCurrentFrom(void* layer_definition);
    virtual void layerWriteCurrentTo(void* layer_definition);
    virtual void autoPresetSelected(int preset);

private:
    CloudsDefinition* _definition;
    CloudLayerDefinition* _layer;

    BasePreview* _previewCoverage;
    CloudsCoveragePreviewRenderer* _previewCoverageRenderer;

    BasePreview* _previewColor;
    CloudsAspectPreviewRenderer* _previewColorRenderer;
};

#endif
