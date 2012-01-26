#include "formtextures.h"

#include "../lib_paysages/textures.h"
#include "../lib_paysages/scenery.h"

static TexturesDefinition _definition;

/**************** Previews ****************/

/**************** Form ****************/
FormTextures::FormTextures(QWidget *parent):
    BaseForm(parent)
{
    _definition = texturesCreateDefinition();

    /*previewHeight = new PreviewTerrainHeight(this);
    previewColor = new PreviewTerrainColor(this);
    addPreview(previewHeight, QString("Height preview (normalized)"));
    addPreview(previewColor, QString("Textured preview (no shadow)"));*/

    /*addInputNoise("Noise", _definition.height_noise);
    addInputDouble("Height", &_definition.height_factor, 0.0, 20.0, 0.1, 1.0);
    addInputDouble("Scaling", &_definition.scaling, 1.0, 20.0, 0.1, 1.0);*/

    revertConfig();
}

void FormTextures::revertConfig()
{
    sceneryGetTextures(&_definition);
    BaseForm::revertConfig();
}

void FormTextures::applyConfig()
{
    scenerySetTextures(&_definition);
    BaseForm::applyConfig();
}

void FormTextures::configChangeEvent()
{
    texturesValidateDefinition(&_definition);
    BaseForm::configChangeEvent();
}
