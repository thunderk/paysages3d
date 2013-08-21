#include "maintexturesform.h"
#include "ui_maintexturesform.h"

#include "../common/freeformhelper.h"
#include "../common/freelayerhelper.h"
#include "rendering/scenery.h"
#include "previewmaterial.h"
#include "editing/textures/PreviewLayerCoverage.h"

MainTexturesForm::MainTexturesForm(QWidget *parent) : QWidget(parent), ui(new Ui::MainTexturesForm)
{
    textures = (TexturesDefinition*) TexturesDefinitionClass.create();

    ui->setupUi(this);

    layer_helper = new FreeLayerHelper(textures->layers, true);
    layer_helper->setLayerTable(ui->layersGrid);
    layer_helper->setAddButton(ui->layer_add);
    layer_helper->setDelButton(ui->layer_del);
    layer_helper->setDownButton(ui->layer_down);
    layer_helper->setUpButton(ui->layer_up);
    connect(layer_helper, SIGNAL(tableUpdateNeeded()), this, SLOT(updateLayers()));
    connect(layer_helper, SIGNAL(selectionChanged(int)), this, SLOT(selectLayer(int)));

    form_helper = new FreeFormHelper(this);
    form_helper->setApplyButton(ui->button_apply);
    form_helper->setRevertButton(ui->button_revert);
    form_helper->setExploreButton(ui->button_explore);
    form_helper->setRenderButton(ui->button_render);
    form_helper->startManaging();

    preview_layer_coverage = new PreviewLayerCoverage();
    preview_layer_coverage->setTextures(textures);
    form_helper->addPreview(ui->preview_coverage, preview_layer_coverage);

    form_helper->addPreset(tr("Rocks with grass"));
    form_helper->addPreset(tr("Snow covered mountains"));
    form_helper->addPreset(tr("Arid canyons"));
    form_helper->setPresetButton(ui->button_preset);
    connect(form_helper, SIGNAL(presetSelected(int)), this, SLOT(selectPreset(int)));

    connect(layer_helper, SIGNAL(layersChanged()), form_helper, SLOT(processDataChange()));
}

MainTexturesForm::~MainTexturesForm()
{
    delete ui;
    delete form_helper;
    delete layer_helper;
}

void MainTexturesForm::updateLayers()
{
    int i, n;

    ui->layersGrid->clearContents();

    n = layersCount(textures->layers);
    ui->layersGrid->setRowCount(n);

    for (i = 0; i < n; i++)
    {
        QTableWidgetItem* item;
        TexturesLayerDefinition* layer = (TexturesLayerDefinition*) layersGetLayer(textures->layers, i);

        item = new QTableWidgetItem(QString("%1").arg(i + 1));
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->layersGrid->setItem(n - 1 - i, 0, item);

        item = new QTableWidgetItem(QString(layersGetName(textures->layers, i)));
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->layersGrid->setItem(n - 1 - i, 1, item);

        ui->layersGrid->setCellWidget(n - 1 - i, 2, new SmallMaterialPreview(ui->layersGrid, &layer->material));
    }

    ui->preview_cumul->setEnabled(n > 0);

    ui->layersGrid->resizeColumnsToContents();
}

void MainTexturesForm::selectLayer(int layer)
{
    if (layer < 0)
    {
        ui->preview_coverage->setEnabled(false);
        ui->preview_texture->setEnabled(false);
    }
    else
    {
        ui->preview_coverage->setEnabled(true);
        ui->preview_texture->setEnabled(true);

        preview_layer_coverage->setLayer(layer);

        ui->preview_coverage->redraw();
        ui->preview_texture->redraw();
    }
}

void MainTexturesForm::selectPreset(int preset)
{
    texturesAutoPreset(textures, (TexturesPreset)preset);
}

void MainTexturesForm::updateLocalDataFromScenery()
{
    sceneryGetTextures(textures);
}

void MainTexturesForm::commitLocalDataToScenery()
{
    scenerySetTextures(textures);
}

void MainTexturesForm::refreshFromLocalData()
{
    layer_helper->refreshLayers();
}

void MainTexturesForm::refreshFromFellowData()
{
}

void MainTexturesForm::alterRenderer(Renderer* renderer)
{
    TexturesRendererClass.bind(renderer, textures);
}
