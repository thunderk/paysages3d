#ifndef _PAYSAGES_QT_FORMCLOUDS_H_
#define _PAYSAGES_QT_FORMCLOUDS_H_

#include <QWidget>
#include "basepreview.h"
#include "baseformlayer.h"
#include "../lib_paysages/clouds.h"

class FormClouds : public BaseFormLayer
{
    Q_OBJECT

public:
    explicit FormClouds(QWidget *parent = 0);

public slots:
    virtual void revertConfig();
    virtual void applyConfig();

protected:
    virtual void layerGetCopy(void* layer_definition);
    virtual void layerApply(void* layer_definition);
    virtual void autoPresetSelected(int preset);

private:
    CloudsDefinition _definition;
    CloudsLayerDefinition* _layer;
    BasePreview* _previewCoverage;
    BasePreview* _previewColor;
};

#endif
