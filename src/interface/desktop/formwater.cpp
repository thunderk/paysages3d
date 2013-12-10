#include "formwater.h"

#include <QColor>
#include <QSlider>
#include <cmath>

#include "tools.h"
#include "DesktopScenery.h"
#include "BasePreview.h"
#include "CameraDefinition.h"
#include "WaterDefinition.h"
#include "WaterCoveragePreviewRenderer.h"
#include "WaterAspectPreviewRenderer.h"

static WaterDefinition* _definition;

/**************** Form ****************/
FormWater::FormWater(QWidget *parent) :
BaseForm(parent)
{
    addAutoPreset(tr("Lake surface"));
    addAutoPreset(tr("Standard sea"));

    _definition = new WaterDefinition(NULL);

    previewCoverage = new BasePreview(this);
    previewCoverageRenderer = new WaterCoveragePreviewRenderer(_definition);
    addPreview(previewCoverage, tr("Coverage preview"));
    previewCoverage->setRenderer(previewCoverageRenderer);

    previewColor = new BasePreview(this);
    previewColorRenderer = new WaterAspectPreviewRenderer(_definition);
    addPreview(previewColor, tr("Rendered preview"));
    previewColor->setRenderer(previewColorRenderer);

    //addInputDouble(tr("Height"), &_definition->height, -15.0, 15.0, 0.1, 1.0);
    addInputMaterial(tr("Surface material"), _definition->material);
    addInputColor(tr("Depth color"), _definition->depth_color);
    addInputDouble(tr("Transparency"), &_definition->transparency, 0.0, 1.0, 0.001, 0.1);
    addInputDouble(tr("Reflection"), &_definition->reflection, 0.0, 1.0, 0.001, 0.1);
    addInputDouble(tr("Transparency distance"), &_definition->transparency_depth, 0.0, 20.0, 0.1, 1.0);
    addInputDouble(tr("Light-through distance"), &_definition->lighting_depth, 0.0, 20.0, 0.1, 1.0);
    addInputDouble(tr("Waves scaling"), &_definition->scaling, 0.02, 2.0, 0.02, 0.2);
    addInputDouble(tr("Waves height"), &_definition->waves_height, 0.0, 2.0, 0.02, 0.2);
    addInputDouble(tr("Waves detail"), &_definition->detail_height, 0.0, 0.3, 0.003, 0.03);
    addInputDouble(tr("Waves turbulence"), &_definition->turbulence, 0.0, 0.5, 0.005, 0.05);
    addInputDouble(tr("Foam coverage"), &_definition->foam_coverage, 0.0, 1.0, 0.01, 0.1);
    addInputMaterial(tr("Foam material"), _definition->foam_material);

    revertConfig();
}

FormWater::~FormWater()
{
    delete _definition;

    delete previewCoverageRenderer;
    delete previewCoverage;

    delete previewColorRenderer;
    delete previewColor;
}

void FormWater::revertConfig()
{
    DesktopScenery::getCurrent()->getWater(_definition);
    BaseForm::revertConfig();
}

void FormWater::applyConfig()
{
    DesktopScenery::getCurrent()->setWater(_definition);
    BaseForm::applyConfig();
}

void FormWater::configChangeEvent()
{
    _definition->validate();
    BaseForm::configChangeEvent();
}

void FormWater::autoPresetSelected(int preset)
{
    _definition->applyPreset((WaterDefinition::WaterPreset)preset);
    BaseForm::autoPresetSelected(preset);
}

