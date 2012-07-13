#include "dialogheightmap.h"
#include "widgetheightmap.h"

#include <QBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QSlider>

/**************** Dialog form ****************/
DialogHeightMap::DialogHeightMap(QWidget* parent, HeightMap* heightmap) : DialogWithPreview(parent)
{
    QWidget* mainarea;
    QWidget* buttons;
    QWidget* panel;
    QWidget* viewer;
    QGridLayout* viewer_layout;
    
    QSlider* slider;
    QPushButton* button;
    
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
    
    // Viewer layout (3d display + sliders)
    _3dview = new WidgetHeightMap(viewer, &_value_modified);
    viewer_layout->addWidget(_3dview, 0, 0);
    slider = new QSlider(Qt::Horizontal, viewer);
    viewer_layout->addWidget(slider, 1, 0);
    slider = new QSlider(Qt::Vertical, viewer);
    viewer_layout->addWidget(slider, 0, 1);
    
    // Panel layout
    button = new QPushButton(tr("Reset to terrain height"), buttons);
    panel->layout()->addWidget(button);

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
    QDialog::accept();
}

void DialogHeightMap::revert()
{
}
