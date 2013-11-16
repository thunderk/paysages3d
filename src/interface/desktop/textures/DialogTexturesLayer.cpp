#include "DialogTexturesLayer.h"
#include "ui_DialogTexturesLayer.h"

#include "TexturesDefinition.h"

DialogTexturesLayer::DialogTexturesLayer(QWidget* parent, TexturesDefinition* textures, int layer) :
    QDialog(parent),
    ui(new Ui::DialogTexturesLayer)
{
    ui->setupUi(this);

    this->layer = layer;
    original = textures;
    modified = new TexturesDefinition(NULL);
}

DialogTexturesLayer::~DialogTexturesLayer()
{
    delete ui;

    delete modified;
}
