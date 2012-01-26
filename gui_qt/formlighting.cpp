#include "formlighting.h"

#include "../lib_paysages/lighting.h"
#include "../lib_paysages/scenery.h"

static LightingDefinition _definition;

/**************** Previews ****************/

/**************** Form ****************/
FormLighting::FormLighting(QWidget *parent):
    BaseForm(parent)
{
    _definition = lightingCreateDefinition();

    /*previewHeight = new PreviewTerrainHeight(this);
    previewColor = new PreviewTerrainColor(this);
    addPreview(previewHeight, QString("Height preview (normalized)"));
    addPreview(previewColor, QString("Textured preview (no shadow)"));*/

    /*addInputNoise("Noise", _definition.height_noise);
    addInputDouble("Height", &_definition.height_factor, 0.0, 20.0, 0.1, 1.0);
    addInputDouble("Scaling", &_definition.scaling, 1.0, 20.0, 0.1, 1.0);*/

    revertConfig();
}

void FormLighting::revertConfig()
{
    sceneryGetLighting(&_definition);
    BaseForm::revertConfig();
}

void FormLighting::applyConfig()
{
    scenerySetLighting(&_definition);
    BaseForm::applyConfig();
}

void FormLighting::configChangeEvent()
{
    lightingValidateDefinition(&_definition);
    BaseForm::configChangeEvent();
}
