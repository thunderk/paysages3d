#include "dialogheightmap.h"

#include <QLabel>
#include <QBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QComboBox>
#include <QSlider>
#include <math.h>
#include "widgetheightmap.h"

/**************** Dialog form ****************/
DialogHeightMap::DialogHeightMap(QWidget* parent, HeightMap* heightmap) : DialogWithPreview(parent)
{
    QWidget* mainarea;
    QWidget* buttons;
    QWidget* panel;
    QWidget* viewer;
    QGridLayout* viewer_layout;
    
    QLabel* label;
    QSlider* slider;
    QPushButton* button;
    QComboBox* combobox;
    
    _value_original = heightmap;
    _value_modified = heightmapCreate();
    heightmapCopy(_value_original, &_value_modified);
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
    viewer = new QWidget(mainarea);
    viewer_layout = new QGridLayout();
    viewer->setLayout(viewer_layout);
    mainarea->layout()->addWidget(viewer);
    
    panel = new QWidget(mainarea);
    panel->setLayout(new QVBoxLayout());
    mainarea->layout()->addWidget(panel);
    mainarea->layout()->setAlignment(panel, Qt::AlignTop);
    
    // Viewer layout (3d display + sliders)
    _3dview = new WidgetHeightMap(viewer, &_value_modified);
    viewer_layout->addWidget(_3dview, 0, 0);
    slider = new QSlider(Qt::Horizontal, viewer);
    slider->setRange(0, 1000);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(angleHChanged(int)));
    viewer_layout->addWidget(slider, 1, 0);
    slider = new QSlider(Qt::Vertical, viewer);
    slider->setRange(-300, 700);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(angleVChanged(int)));
    viewer_layout->addWidget(slider, 0, 1);
    
    // Panel layout
    button = new QPushButton(tr("Reset to terrain height"), panel);
    connect(button, SIGNAL(clicked()), _3dview, SLOT(resetToTerrain()));
    panel->layout()->addWidget(button);
    
    combobox = new QComboBox(panel);
    combobox->addItem(tr("Raise / lower"));
    combobox->addItem(tr("Smooth / add noise"));
    connect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(brushModeChanged(int)));
    panel->layout()->addWidget(combobox);
    
    label = new QLabel(tr("Brush size"), panel);
    panel->layout()->addWidget(label);
    
    slider = new QSlider(Qt::Horizontal, panel);
    slider->setRange(6, 150);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(brushSizeChanged(int)));
    panel->layout()->addWidget(slider);
    slider->setValue(30);

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
    button = new QPushButton(tr("Validate"), buttons);
    buttons->layout()->addWidget(button);
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(accept()));

    button = new QPushButton(tr("Revert"), buttons);
    buttons->layout()->addWidget(button);
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(revert()));

    button = new QPushButton(tr("Cancel"), buttons);
    buttons->layout()->addWidget(button);
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(reject()));

    setWindowTitle(tr("Paysages 3D - Height map painting"));
}

bool DialogHeightMap::editHeightMap(QWidget* parent, HeightMap* heightmap)
{
    int result;

    DialogHeightMap* dialog = new DialogHeightMap(parent, heightmap);
    result = dialog->exec();

    delete dialog;

    return (result != 0) ? true : false;
}

void DialogHeightMap::accept()
{
    heightmapCopy(&_value_modified, _value_original);
    QDialog::accept();
}

void DialogHeightMap::revert()
{
    heightmapCopy(_value_original, &_value_modified);
    _3dview->revert();
}

void DialogHeightMap::angleHChanged(int value)
{
    _3dview->setHorizontalViewAngle(M_PI * ((double)value) / 500.0);
}

void DialogHeightMap::angleVChanged(int value)
{
    _3dview->setVerticalViewAngle(M_PI_2 * ((double)value) / 1000.0);
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
