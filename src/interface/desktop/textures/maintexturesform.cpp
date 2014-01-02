#include "maintexturesform.h"
#include "ui_maintexturesform.h"

#include "../common/freeformhelper.h"
#include "../common/freelayerhelper.h"
#include "DesktopScenery.h"
#include "TexturesDefinition.h"
#include "TextureLayerDefinition.h"
#include "previewmaterial.h"
#include "textures/DialogTexturesLayer.h"
#include "TexturesMixPreviewRenderer.h"
#include "TextureLayerCoveragePreviewRenderer.h"
#include "TextureLayerAspectPreviewRenderer.h"

MainTexturesForm::MainTexturesForm(QWidget *parent) : QWidget(parent), ui(new Ui::MainTexturesForm)
{
    textures = new TexturesDefinition(NULL);

    ui->setupUi(this);

    layer_helper = new FreeLayerHelper(textures, true);
    layer_helper->setLayerTable(ui->layersGrid);
    layer_helper->setAddButton(ui->layer_add);
    layer_helper->setDelButton(ui->layer_del);
    layer_helper->setDownButton(ui->layer_down);
    layer_helper->setUpButton(ui->layer_up);
    layer_helper->setEditButton(ui->layer_edit);
    connect(layer_helper, SIGNAL(tableUpdateNeeded()), this, SLOT(updateLayers()));
    connect(layer_helper, SIGNAL(selectionChanged(int)), this, SLOT(selectLayer(int)));
    connect(layer_helper, SIGNAL(editRequired(int)), this, SLOT(editLayer(int)));

    form_helper = new FreeFormHelper(this);
    form_helper->setApplyButton(ui->button_apply);
    form_helper->setRevertButton(ui->button_revert);
    form_helper->setExploreButton(ui->button_explore);
    form_helper->setRenderButton(ui->button_render);

    preview_layer_coverage = new TextureLayerCoveragePreviewRenderer();
    preview_layer_coverage->setTextures(textures);
    form_helper->addPreview(ui->preview_coverage, preview_layer_coverage);

    preview_layer_look = new TextureLayerAspectPreviewRenderer();
    preview_layer_look->setTextures(textures);
    form_helper->addPreview(ui->preview_texture, preview_layer_look);

    preview_cumul = new TexturesMixPreviewRenderer();
    preview_cumul->setTextures(textures);
    form_helper->addPreview(ui->preview_cumul, preview_cumul);

    form_helper->addPreset(tr("Complex terrain"));
    form_helper->addPreset(tr("Rocks with grass"));
    form_helper->addPreset(tr("Snow covered mountains"));
    //form_helper->addPreset(tr("Arid canyons"));
    form_helper->setPresetButton(ui->button_preset);
    connect(form_helper, SIGNAL(presetSelected(int)), this, SLOT(selectPreset(int)));

    connect(layer_helper, SIGNAL(layersChanged()), form_helper, SLOT(processDataChange()));

    form_helper->startManaging();
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

    n = textures->count();
    ui->layersGrid->setRowCount(n);

    for (i = 0; i < n; i++)
    {
        QTableWidgetItem* item;
        TextureLayerDefinition* layer = textures->getTextureLayer(i);

        item = new QTableWidgetItem(QString("%1").arg(i + 1));
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->layersGrid->setItem(n - 1 - i, 0, item);

        item = new QTableWidgetItem(QString::fromStdString(layer->getName()));
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->layersGrid->setItem(n - 1 - i, 1, item);

        QWidget* widget = new SmallMaterialPreview(ui->layersGrid, layer->material);
        //widget->setMinimumSize(50, 50);
        ui->layersGrid->setCellWidget(n - 1 - i, 2, widget);

        ui->layersGrid->setRowHeight(n - 1 - i, 50);
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
        preview_layer_look->setLayer(layer);

        ui->preview_coverage->redraw();
        ui->preview_texture->redraw();
    }
}

void MainTexturesForm::editLayer(int layer)
{
    DialogTexturesLayer dialog(this, textures, layer);
    dialog.exec();
}

void MainTexturesForm::selectPreset(int preset)
{
    textures->applyPreset((TexturesDefinition::TexturesPreset)preset);
}

void MainTexturesForm::updateLocalDataFromScenery()
{
    DesktopScenery::getCurrent()->getTextures(textures);

    preview_layer_coverage->setTerrain(DesktopScenery::getCurrent()->getTerrain());
    preview_cumul->setTerrain(DesktopScenery::getCurrent()->getTerrain());
}

void MainTexturesForm::commitLocalDataToScenery()
{
    DesktopScenery::getCurrent()->setTextures(textures);
}

void MainTexturesForm::refreshFromLocalData()
{
    layer_helper->refreshLayers();
}

void MainTexturesForm::refreshFromFellowData()
{
}

void MainTexturesForm::alterRenderer(SoftwareRenderer* renderer)
{
    renderer->getScenery()->setTextures(textures);
    renderer->prepare();
}
