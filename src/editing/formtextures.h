#ifndef _PAYSAGES_QT_FORMTEXTURES_H_
#define _PAYSAGES_QT_FORMTEXTURES_H_

#include <QWidget>
#include "basepreview.h"
#include "baseformlayer.h"
#include "rendering/textures/public.h"

class FormTextures : public BaseFormLayer
{
    Q_OBJECT

public:
    explicit FormTextures(QWidget *parent = 0);
    ~FormTextures();

public slots:
    virtual void revertConfig();
    virtual void applyConfig();

protected:
    virtual void layerReadCurrentFrom(void* layer_definition);
    virtual void layerWriteCurrentTo(void* layer_definition);
    virtual void autoPresetSelected(int preset);

private:
    TexturesDefinition* _definition;
    TexturesLayerDefinition* _layer;
    BasePreview* _previewCoverage;
    BasePreview* _previewColor;
};

#endif
