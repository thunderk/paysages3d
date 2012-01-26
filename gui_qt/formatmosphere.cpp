#include "formatmosphere.h"

#include "../lib_paysages/atmosphere.h"
#include "../lib_paysages/scenery.h"

static AtmosphereDefinition _definition;

/**************** Previews ****************/

/**************** Form ****************/
FormAtmosphere::FormAtmosphere(QWidget *parent):
    BaseForm(parent)
{
    _definition = atmosphereCreateDefinition();

    /*previewHeight = new PreviewTerrainHeight(this);
    previewColor = new PreviewTerrainColor(this);
    addPreview(previewHeight, QString("Height preview (normalized)"));
    addPreview(previewColor, QString("Textured preview (no shadow)"));*/

    /*addInputNoise("Noise", _definition.height_noise);
    addInputDouble("Height", &_definition.height_factor, 0.0, 20.0, 0.1, 1.0);
    addInputDouble("Scaling", &_definition.scaling, 1.0, 20.0, 0.1, 1.0);*/

    revertConfig();
}

void FormAtmosphere::revertConfig()
{
    sceneryGetAtmosphere(&_definition);
    BaseForm::revertConfig();
}

void FormAtmosphere::applyConfig()
{
    scenerySetAtmosphere(&_definition);
    BaseForm::applyConfig();
}

void FormAtmosphere::configChangeEvent()
{
    atmosphereValidateDefinition(&_definition);
    BaseForm::configChangeEvent();
}
