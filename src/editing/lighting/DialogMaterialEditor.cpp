#include "DialogMaterialEditor.h"
#include "ui_DialogMaterialEditor.h"

#include "editing/common/freeformhelper.h"

DialogMaterialEditor::DialogMaterialEditor(QWidget *parent, SurfaceMaterial* material) :
QDialog(parent),
ui(new Ui::DialogMaterialEditor),
preview_lighted(&edited)
{
    ui->setupUi(this);

    original = material;
    edited = *original;

    form_helper = new FreeFormHelper(this);

    form_helper->addPreview(ui->preview_lighted, &preview_lighted);

    form_helper->addDoubleInputSlider(ui->slider_hue, &edited.base.h);
    form_helper->addDoubleInputSlider(ui->slider_lightness, &edited.base.l);
    form_helper->addDoubleInputSlider(ui->slider_saturation, &edited.base.s);

    form_helper->addDoubleInputSlider(ui->slider_hardness, &edited.hardness);
    form_helper->addDoubleInputSlider(ui->slider_reflection, &edited.reflection);
    form_helper->addDoubleInputSlider(ui->slider_specularity, &edited.shininess);

    form_helper->addDoubleInputSlider(ui->slider_receive_shadows, &edited.receive_shadows);

    form_helper->setRevertButton(ui->button_revert);

    form_helper->startManaging();

    ui->preview_color->setColor(&edited._rgb);

    connect(ui->button_apply, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->button_cancel, SIGNAL(clicked()), this, SLOT(reject()));
}

DialogMaterialEditor::~DialogMaterialEditor()
{
    delete form_helper;
    delete ui;
}

bool DialogMaterialEditor::getMaterial(QWidget* parent, SurfaceMaterial* material)
{
    DialogMaterialEditor dialog(parent, material);
    return dialog.exec() != 0;
}

void DialogMaterialEditor::refreshFromLocalData()
{
    materialValidate(&edited);
    ui->preview_color->update();
}

void DialogMaterialEditor::refreshFromFellowData()
{
}

void DialogMaterialEditor::updateLocalDataFromScenery()
{
    // Revert
    edited = *original;
}

void DialogMaterialEditor::commitLocalDataToScenery()
{
}

void DialogMaterialEditor::alterRenderer(Renderer*)
{
}
