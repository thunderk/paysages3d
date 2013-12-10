#include "mainterrainform.h"
#include "ui_mainterrainform.h"

#include <QMessageBox>
#include "dialogbaseterrainnoise.h"
#include "dialogterrainpainting.h"
#include "TerrainShapePreviewRenderer.h"
#include "common/freeformhelper.h"
#include "tools.h"
#include "DesktopScenery.h"
#include "TerrainDefinition.h"
#include "TerrainHeightMap.h"

MainTerrainForm::MainTerrainForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainTerrainForm)
{
    _terrain = new TerrainDefinition(NULL);

    ui->setupUi(this);

    // TEMP
    ui->button_dialog_basenoise->hide();

    _form_helper = new FreeFormHelper(this);

    _renderer_shape = new TerrainShapePreviewRenderer(_terrain);
    _form_helper->addPreview(ui->preview_shape, _renderer_shape);

    _form_helper->addDoubleInputSlider(ui->input_scaling, &_terrain->scaling, 0.1, 3.0, 0.03, 0.3);
    _form_helper->addDoubleInputSlider(ui->input_height, &_terrain->height, 1.0, 90.0, 0.5, 5.0);
    _form_helper->addDoubleInputSlider(ui->input_shadow_smoothing, &_terrain->shadow_smoothing, 0.0, 0.3, 0.003, 0.03);
    _form_helper->addDoubleInputSlider(ui->input_water_height, &_terrain->water_height, -1.0, 1.0, 0.01, 0.1);

    _form_helper->setApplyButton(ui->button_apply);
    _form_helper->setRevertButton(ui->button_revert);
    _form_helper->setExploreButton(ui->button_explore);
    _form_helper->setRenderButton(ui->button_render);

    connect(ui->button_dialog_basenoise, SIGNAL(clicked()), this, SLOT(buttonBaseNoisePressed()));
    connect(ui->button_dialog_painting, SIGNAL(clicked()), this, SLOT(buttonPaintingPressed()));
    connect(ui->button_goto_textures, SIGNAL(clicked()), this, SLOT(buttonTexturesPressed()));

    _form_helper->startManaging();
}

MainTerrainForm::~MainTerrainForm()
{
    delete _form_helper;
    delete ui;
    delete _renderer_shape;

    delete _terrain;
}

void MainTerrainForm::refreshFromLocalData()
{
    unsigned long memused = _terrain->getMemoryStats();
    if (memused > 0)
    {
        _form_helper->setLabelText(ui->label_painting_info, tr("Memory used by sculpted data: %1").arg(getHumanMemory(memused)));
    }
    else
    {
        _form_helper->setLabelText(ui->label_painting_info, tr("No manual scuplting done"));
    }

    ui->widget_base_noise_preview->setNoise(_terrain->_height_noise);
}

void MainTerrainForm::refreshFromFellowData()
{
    //double disp = texturesGetMaximalDisplacement(RenderingScenery::getCurrent()->getTextures());
    double disp = -1000.0;

    if (disp == 0.0)
    {
        ui->label_info_textures->setText(tr("No displacement textures"));
    }
    else
    {
        ui->label_info_textures->setText(tr("Maximal displacement : %1% of total height").arg(100.0 * disp / _terrain->height, 0, 'f', 1));
    }
}

void MainTerrainForm::updateLocalDataFromScenery()
{
    DesktopScenery::getCurrent()->getTerrain(_terrain);
}

void MainTerrainForm::commitLocalDataToScenery()
{
    DesktopScenery::getCurrent()->setTerrain(_terrain);
}

void MainTerrainForm::alterRenderer(SoftwareRenderer* renderer)
{
    renderer->getScenery()->setTerrain(_terrain);
    renderer->prepare();
}

void MainTerrainForm::buttonBaseNoisePressed()
{
    int erase;
    if (_terrain->getMemoryStats() > 0)
    {
        erase = QMessageBox::question(this, tr("Paysages 3D - Base noise edition"), tr("You have manual modifications on this terrain, regenerating base noise may produce weird results."), tr("Keep my changes anyway"), tr("Erase my changes"));
    }
    else
    {
        erase = 0;
    }

    if (DialogBaseTerrainNoise::editNoise(this, _terrain->_height_noise))
    {
        if (erase)
        {
            _terrain->height_map->clearPainting();
        }
    }
}

void MainTerrainForm::buttonPaintingPressed()
{
    DialogTerrainPainting dialog(this, _terrain);
    _form_helper->openDialog(&dialog);
}

void MainTerrainForm::buttonTexturesPressed()
{
    // Switch to textures tab
    _form_helper->gotoMainTab(1);
}
