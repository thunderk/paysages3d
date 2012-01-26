#include "formclouds.h"

#include "../lib_paysages/clouds.h"
#include "../lib_paysages/scenery.h"

static CloudsDefinition _definition;

/**************** Previews ****************/

/**************** Form ****************/
FormClouds::FormClouds(QWidget *parent):
    BaseForm(parent)
{
    _definition = cloudsCreateDefinition();

    /*previewHeight = new PreviewTerrainHeight(this);
    previewColor = new PreviewTerrainColor(this);
    addPreview(previewHeight, QString("Height preview (normalized)"));
    addPreview(previewColor, QString("Textured preview (no shadow)"));*/

    /*addInputNoise("Noise", _definition.height_noise);
    addInputDouble("Height", &_definition.height_factor, 0.0, 20.0, 0.1, 1.0);
    addInputDouble("Scaling", &_definition.scaling, 1.0, 20.0, 0.1, 1.0);*/

    revertConfig();
}

void FormClouds::revertConfig()
{
    sceneryGetClouds(&_definition);
    BaseForm::revertConfig();
}

void FormClouds::applyConfig()
{
    scenerySetClouds(&_definition);
    BaseForm::applyConfig();
}

void FormClouds::configChangeEvent()
{
    cloudsValidateDefinition(&_definition);
    BaseForm::configChangeEvent();
}
