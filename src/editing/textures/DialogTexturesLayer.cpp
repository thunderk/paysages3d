#include "DialogTexturesLayer.h"
#include "ui_DialogTexturesLayer.h"

DialogTexturesLayer::DialogTexturesLayer(QWidget* parent, TexturesDefinition* textures, int layer) :
    QDialog(parent),
    ui(new Ui::DialogTexturesLayer)
{
    ui->setupUi(this);

    this->layer = layer;
    original = textures;
    modified = (TexturesDefinition*)TexturesDefinitionClass.create();
}

DialogTexturesLayer::~DialogTexturesLayer()
{
    delete ui;

    TexturesDefinitionClass.destroy(modified);
}
