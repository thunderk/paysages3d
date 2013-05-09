#include "mainterrainform.h"
#include "ui_mainterrainform.h"

#include "dialogterrainpainting.h"
#include "tools.h"
#include "rendering/scenery.h"

MainTerrainForm::MainTerrainForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainTerrainForm)
{
    _terrain = (TerrainDefinition*)TerrainDefinitionClass.create();

    ui->setupUi(this);

    _form_helper = new FreeFormHelper(this);

    _form_helper->addPreview("preview_shape");

    _form_helper->addDoubleInputSlider("input_scaling", &_terrain->scaling, 0.1, 3.0, 0.03, 0.3);
    _form_helper->addDoubleInputSlider("input_height", &_terrain->height, 0.0, 3.0, 0.01, 0.1);
    _form_helper->addDoubleInputSlider("input_shadow_smoothing", &_terrain->shadow_smoothing, 0.0, 0.3, 0.003, 0.03);

    _form_helper->setApplyButton("button_apply");
    _form_helper->setRevertButton("button_revert");

    connect(findChild<QPushButton*>("button_dialog_painting"), SIGNAL(clicked()), this, SLOT(buttonPaintingPressed()));
    connect(findChild<QPushButton*>("button_goto_textures"), SIGNAL(clicked()), this, SLOT(buttonTexturesPressed()));

    _form_helper->startManaging();
}

MainTerrainForm::~MainTerrainForm()
{
    delete _form_helper;
    delete ui;

    TerrainDefinitionClass.destroy(_terrain);
}

void MainTerrainForm::refreshFromLocalData()
{
    qint64 memused = terrainGetMemoryStats(_terrain);
    if (memused > 0)
    {
        _form_helper->setLabelText("label_painting_info", tr("Memory used by sculpted data: %1").arg(getHumanMemory(memused)));
    }
    else
    {
        _form_helper->setLabelText("label_painting_info", tr("No manual scuplting done"));
    }
}

void MainTerrainForm::refreshFromFellowData()
{
    // TODO Refresh texture info
}

void MainTerrainForm::updateLocalDataFromScenery()
{
    sceneryGetTerrain(_terrain);
}

void MainTerrainForm::commitLocalDataToScenery()
{
    scenerySetTerrain(_terrain);
}

void MainTerrainForm::buttonPaintingPressed()
{
    DialogTerrainPainting dialog(this, _terrain);
    _form_helper->openDialog(&dialog);
}

void MainTerrainForm::buttonTexturesPressed()
{
    // TODO Switch to textures tab
}