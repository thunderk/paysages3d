#include "dialogheightmap.h"

#include <QLabel>
#include <QBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QComboBox>
#include <QSlider>
#include <QFileDialog>
#include <QInputDialog>
#include <math.h>
#include "rendering/scenery.h"
#include "widgetheightmap.h"

/**************** Dialog form ****************/
DialogHeightMap::DialogHeightMap(QWidget* parent, TerrainDefinition* terrain) : DialogWithPreview(parent)
{
    QWidget* mainarea;
    QWidget* buttons;
    QWidget* panel;

    QLabel* label;
    QSlider* slider;
    QPushButton* button;
    QComboBox* combobox;

    _value_original = terrain;
    _value_modified = (TerrainDefinition*)TerrainDefinitionClass.create();
    TerrainDefinitionClass.copy(_value_original, _value_modified);
    setLayout(new QVBoxLayout());

    // Dialog layout (main area + buttons)
    mainarea = new QWidget(this);
    mainarea->setLayout(new QHBoxLayout());
    this->layout()->addWidget(mainarea);

    buttons = new QWidget(this);
    buttons->setLayout(new QHBoxLayout());
    buttons->layout()->setAlignment(buttons, Qt::AlignBottom);
    this->layout()->addWidget(buttons);

    // Main area layout (viewer + panel)
    _3dview = new WidgetHeightMap(mainarea, _value_modified);
    connect(_3dview, SIGNAL(heightmapChanged()), this, SLOT(heightmapChanged()));
    mainarea->layout()->addWidget(_3dview);

    panel = new QWidget(mainarea);
    panel->setLayout(new QVBoxLayout());
    mainarea->layout()->addWidget(panel);
    mainarea->layout()->setAlignment(panel, Qt::AlignTop);

    // Panel layout
    _info_memory = new QLabel(panel);
    panel->layout()->addWidget(_info_memory);

    /*button = new QPushButton(tr("Load from picture file"), panel);
    connect(button, SIGNAL(clicked()), this, SLOT(loadFromFile()));
    panel->layout()->addWidget(button);*/

    combobox = new QComboBox(panel);
    combobox->addItem(tr("Raise / lower"));
    combobox->addItem(tr("Add noise / smooth"));
    combobox->addItem(tr("Restore to original"));
    connect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(brushModeChanged(int)));
    panel->layout()->addWidget(combobox);

    label = new QLabel(tr("Brush size"), panel);
    panel->layout()->addWidget(label);

    slider = new QSlider(Qt::Horizontal, panel);
    slider->setRange(6, 300);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(brushSizeChanged(int)));
    panel->layout()->addWidget(slider);
    slider->setValue(60);

    label = new QLabel(tr("Brush smoothing"), panel);
    panel->layout()->addWidget(label);

    slider = new QSlider(Qt::Horizontal, panel);
    slider->setRange(0, 1000);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(brushSmoothingChanged(int)));
    panel->layout()->addWidget(slider);
    slider->setValue(600);

    label = new QLabel(tr("Brush strength"), panel);
    panel->layout()->addWidget(label);

    slider = new QSlider(Qt::Horizontal, panel);
    slider->setRange(0, 1000);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(brushStrengthChanged(int)));
    panel->layout()->addWidget(slider);
    slider->setValue(200);

    // Buttons layout
    button = new QPushButton(tr("Cancel"), buttons);
    button->setIcon(QIcon(getDataPath("images/cancel.png")));
    buttons->layout()->addWidget(button);
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(reject()));

    button = new QPushButton(tr("Revert"), buttons);
    button->setIcon(QIcon(getDataPath("images/revert.png")));
    buttons->layout()->addWidget(button);
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(revert()));

    button = new QPushButton(tr("Validate"), buttons);
    button->setIcon(QIcon(getDataPath("images/apply.png")));
    buttons->layout()->addWidget(button);
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(accept()));

    setWindowTitle(tr("Paysages 3D - Height map painting"));
}

bool DialogHeightMap::editHeightMap(QWidget* parent, TerrainDefinition* terrain)
{
    int result;

    DialogHeightMap* dialog = new DialogHeightMap(parent, terrain);
    result = dialog->exec();

    delete dialog;

    return (result != 0) ? true : false;
}

void DialogHeightMap::accept()
{
    TerrainDefinitionClass.copy(_value_modified, _value_original);
    QDialog::accept();
}

void DialogHeightMap::revert()
{
    TerrainDefinitionClass.copy(_value_original, _value_modified);
    _3dview->revert();
}

void DialogHeightMap::brushModeChanged(int value)
{
    _3dview->setBrushMode((HeightMapBrushMode)value);
}

void DialogHeightMap::brushSizeChanged(int value)
{
    _3dview->setBrushSize((double)value / 10.0);
}

void DialogHeightMap::brushSmoothingChanged(int value)
{
    _3dview->setBrushSmoothing((double)value / 1000.0);
}

void DialogHeightMap::brushStrengthChanged(int value)
{
    _3dview->setBrushStrength((double)value / 2000.0);
}

void DialogHeightMap::heightmapChanged()
{
    _info_memory->setText(tr("Memory used: %1").arg(_3dview->getMemoryStats()));
}

/*void DialogHeightMap::loadFromFile()
{
    QString filepath = QFileDialog::getOpenFileName(this, tr("Paysages 3D - Choose a picture to load"), QString(), tr("Images (*.jpg *.jpeg *.bmp *.png)"));
    if (!filepath.isNull())
    {
        heightmapImportFromPicture(&_value_modified, (char*) filepath.toStdString().c_str());
        _3dview->revert();
        updateResolutionLabel();
    }
}*/
