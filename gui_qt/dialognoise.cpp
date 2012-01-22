#include "dialognoise.h"

#include <QVBoxLayout>
#include <QImage>
#include <QLabel>
#include <QColor>
#include <QPainter>
#include <QSlider>
#include <QScrollArea>
#include <QPushButton>

#include "../lib_paysages/shared/functions.h"

/**************** Previews ****************/
class PreviewLevel:public Preview
{
public:
    PreviewLevel(QWidget* parent, NoiseGenerator* noise):
        Preview(parent)
    {
        _noise = noise;
        _level = -1;
    }

    void setLevel(int row)
    {
        _level = row;
        redraw();
    }
protected:
    QColor getColor(double x, double y)
    {
        if ((_level >= 0) && (y > noiseGet1DLevel(_noise, _level, x)))
        {
            return QColor(255, 255, 255);
        }
        else
        {
            return QColor(0, 0, 0);
        }
    }
private:
    NoiseGenerator* _noise;
    int _level;
};

class PreviewTotal:public Preview
{
public:
    PreviewTotal(QWidget* parent, NoiseGenerator* noise):
        Preview(parent),
        _noise(noise)
    {
    }
protected:
    QColor getColor(double x, double y)
    {
        if (y > noiseGet1DTotal(_noise, x))
        {
            return QColor(255, 255, 255);
        }
        else
        {
            return QColor(0, 0, 0);
        }
    }
private:
    NoiseGenerator* _noise;
};

/**************** Dialog form ****************/
DialogNoise::DialogNoise(QWidget *parent, NoiseGenerator* value):
    QDialog(parent)
{
    QWidget* previews;
    QWidget* form;
    QWidget* buttons;
    QPushButton* button;

    _base = value;
    _current = noiseCreateGenerator();

    setLayout(new QHBoxLayout());

    previews = new QWidget(this);
    previews->setLayout(new QVBoxLayout());
    layout()->addWidget(previews);

    previewLevel = new PreviewLevel(previews, _current);
    previews->layout()->addWidget(new QLabel("Level preview"));
    previews->layout()->addWidget(previewLevel);
    previewLevel->setScaling(1.0 / 127.0);
    previewTotal = new PreviewTotal(previews, _current);
    previews->layout()->addWidget(new QLabel("Total preview"));
    previews->layout()->addWidget(previewTotal);
    previewTotal->setScaling(1.0 / 127.0);

    form = new QWidget(this);
    form->setLayout(new QVBoxLayout());
    layout()->addWidget(form);

    form->layout()->addWidget(new QLabel("Noise components"));
    levels = new QListWidget(form);
    form->layout()->addWidget(levels);
    QObject::connect(levels, SIGNAL(currentRowChanged(int)), this, SLOT(levelChanged(int)));

    buttons = new QWidget(form);
    buttons->setLayout(new QHBoxLayout());
    form->layout()->addWidget(buttons);

    button = new QPushButton("Add component", buttons);
    buttons->layout()->addWidget(button);
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(addLevel()));

    button = new QPushButton("Remove component", buttons);
    buttons->layout()->addWidget(button);
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(removeLevel()));

    form->layout()->addWidget(new QLabel("Component height"));
    slider_height = new QSlider(form);
    slider_height->setOrientation(Qt::Horizontal);
    slider_height->setMinimumWidth(150);
    slider_height->setMaximumWidth(400);
    slider_height->setMinimum(0);
    slider_height->setMaximum(1000);
    slider_height->setTickInterval(100);
    slider_height->setTickPosition(QSlider::TicksBelow);
    form->layout()->addWidget(slider_height);
    QObject::connect(slider_height, SIGNAL(valueChanged(int)), this, SLOT(heightChanged(int)));

    form->layout()->addWidget(new QLabel("Component scaling"));
    slider_scaling = new QSlider(form);
    slider_scaling->setOrientation(Qt::Horizontal);
    slider_scaling->setMinimumWidth(150);
    slider_scaling->setMaximumWidth(400);
    slider_scaling->setMinimum(1);
    slider_scaling->setMaximum(1000);
    slider_scaling->setTickInterval(100);
    slider_scaling->setTickPosition(QSlider::TicksBelow);
    form->layout()->addWidget(slider_scaling);
    QObject::connect(slider_scaling, SIGNAL(valueChanged(int)), this, SLOT(scalingChanged(int)));

    buttons = new QWidget(form);
    buttons->setLayout(new QHBoxLayout());
    form->layout()->addWidget(buttons);

    button = new QPushButton("Validate", buttons);
    buttons->layout()->addWidget(button);
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(accept()));

    button = new QPushButton("Reset", buttons);
    buttons->layout()->addWidget(button);
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(revert()));

    button = new QPushButton("Cancel", buttons);
    buttons->layout()->addWidget(button);
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(reject()));

    setWindowTitle("Paysages 3D - Noise editor");

    revert();
}

DialogNoise::~DialogNoise()
{
    delete previewLevel;
    delete previewTotal;

    noiseDeleteGenerator(_current);
}

bool DialogNoise::getNoise(QWidget* parent, NoiseGenerator* noise)
{
    int result;

    DialogNoise* dialog = new DialogNoise(parent, noise);
    result = dialog->exec();

    delete dialog;

    return (result != 0) ? true : false;
}

void DialogNoise::closeEvent(QCloseEvent* e)
{
    reject();
}

void DialogNoise::accept()
{
    noiseCopy(_current, _base);
    QDialog::accept();
}

void DialogNoise::revert()
{
    noiseCopy(_base, _current);

    revertToCurrent();
}

void DialogNoise::revertToCurrent()
{
    int i, n;

    levels->clear();
    n = noiseGetLevelCount(_current);
    for (i = 0; i < n; i++)
    {
        levels->addItem(QString("Component %1").arg(i + 1));
    }

    previewLevel->redraw();
    previewTotal->redraw();
}

void DialogNoise::addLevel()
{
    NoiseLevel level;

    level.height = 0.1;
    level.scaling = 0.1;
    level.xoffset = 0.0;
    level.yoffset = 0.0;
    level.zoffset = 0.0;
    noiseAddLevel(_current, level);

    revertToCurrent();
}

void DialogNoise::removeLevel()
{
    noiseRemoveLevel(_current, _current_level);

    revertToCurrent();
}

void DialogNoise::levelChanged(int row)
{
    if (noiseGetLevel(_current, row, &_current_level_params))
    {
        _current_level = row;
        ((PreviewLevel*)previewLevel)->setLevel(row);

        slider_height->setValue(_current_level_params.height * 1000.0);
        slider_scaling->setValue(_current_level_params.scaling * 1000.0);
    }
    // TODO else ...
}

void DialogNoise::heightChanged(int value)
{
    _current_level_params.height = ((double)value) / 1000.0;
    noiseSetLevel(_current, _current_level, _current_level_params);
    previewLevel->redraw();
    previewTotal->redraw();
}

void DialogNoise::scalingChanged(int value)
{
    _current_level_params.scaling = ((double)value) / 1000.0;
    noiseSetLevel(_current, _current_level, _current_level_params);
    previewLevel->redraw();
    previewTotal->redraw();
}
