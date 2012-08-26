#ifndef _PAYSAGES_QT_FORMTEXTURES_H_
#define _PAYSAGES_QT_FORMTEXTURES_H_

#include <QWidget>
#include "basepreview.h"
#include "baseformlayer.h"
#include "../lib_paysages/textures.h"

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
    virtual void layerGetCopy(void* layer_definition);
    virtual void layerApply(void* layer_definition);

private:
    TexturesDefinition _definition;
    TextureLayerDefinition* _layer;
    BasePreview* previewCoverage;
    BasePreview* previewColor;
};

#endif
