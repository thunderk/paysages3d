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
#include "../lib_paysages/terrain.h"
#include "../lib_paysages/scenery.h"
#include "widgetheightmap.h"

/**************** Dialog form ****************/
DialogHeightMap::DialogHeightMap(QWidget* parent, HeightMap* heightmap, TerrainCanvas* canvas) : DialogWithPreview(parent)
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
    
    _canvas = canvas;
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
    if (canvas)
    {
        button = new QPushButton(tr("Reset to terrain height"), panel);
        connect(button, SIGNAL(clicked()), this, SLOT(resetToTerrain()));
        panel->layout()->addWidget(button);
    }

    button = new QPushButton(tr("Load from picture file"), panel);
    connect(button, SIGNAL(clicked()), this, SLOT(loadFromFile()));
    panel->layout()->addWidget(button);

    button = new QPushButton(tr("Change resolution"), panel);
    connect(button, SIGNAL(clicked()), this, SLOT(changeResolution()));
    panel->layout()->addWidget(button);
    
    _resolution_label = new QLabel("", panel);
    panel->layout()->addWidget(_resolution_label);
    updateResolutionLabel();
    
    combobox = new QComboBox(panel);
    combobox->addItem(tr("Raise / lower"));
    combobox->addItem(tr("Add noise / smooth"));
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

bool DialogHeightMap::editHeightMap(QWidget* parent, HeightMap* heightmap, TerrainCanvas* canvas)
{
    int result;

    DialogHeightMap* dialog = new DialogHeightMap(parent, heightmap, canvas);
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
    updateResolutionLabel();
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

void DialogHeightMap::loadFromFile()
{
    QString filepath = QFileDialog::getOpenFileName(this, tr("Paysages 3D - Choose a picture to load"), QString(), tr("Images (*.jpg *.jpeg *.bmp *.png)"));
    if (!filepath.isNull())
    {
        heightmapImportFromPicture(&_value_modified, (char*) filepath.toStdString().c_str());
        _3dview->revert();
        updateResolutionLabel();
    }
}

void DialogHeightMap::resetToTerrain()
{
    if (_canvas)
    {
        TerrainDefinition terrain;

        terrain = terrainCreateDefinition();
        sceneryGetTerrain(&terrain);

        heightmapRevertToTerrain(&_value_modified, &terrain, &_canvas->area);

        terrainDeleteDefinition(&terrain);
        
        _3dview->revert();
    }
}

void DialogHeightMap::changeResolution()
{
    QString result;
    QStringList items;
    int current;
    
    items << QString("64 x 64") << QString("128 x 128") << QString("256 x 256") << QString("512 x 512");
    current = 1;
    if (_value_modified.resolution_x == 64 && _value_modified.resolution_z == 64)
    {
        current = 0;
    }
    else if (_value_modified.resolution_x == 256 && _value_modified.resolution_z == 256)
    {
        current = 2;
    }
    else if (_value_modified.resolution_x == 512 && _value_modified.resolution_z == 512)
    {
        current = 3;
    }
    result = QInputDialog::getItem(this, tr("Paysages 3D - Change heightmap resolution"), tr("Choose the new heightmap resolution. Beware that lowering the resolution may imply a loss of accuracy."), items, current, false);
    if (!result.isEmpty())
    {
        int new_res_x, new_res_z;
        if (result == QString("64 x 64"))
        {
            new_res_x = new_res_z = 64;
        }
        else if (result == QString("256 x 256"))
        {
            new_res_x = new_res_z = 256;
        }
        else if (result == QString("512 x 512"))
        {
            new_res_x = new_res_z = 512;
        }
        else
        {
            new_res_x = new_res_z = 128;
        }
        
        if (new_res_x != _value_modified.resolution_x or new_res_z != _value_modified.resolution_z)
        {
            heightmapChangeResolution(&_value_modified, new_res_x, new_res_z);
            _3dview->revert();
            updateResolutionLabel();
        }
    }
}

void DialogHeightMap::updateResolutionLabel()
{
    _resolution_label->setText(tr("Map resolution : %1 x %2").arg(_value_modified.resolution_x).arg(_value_modified.resolution_z));
}
