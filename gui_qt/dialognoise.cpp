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
        Preview(parent),
        _noise(noise)
    {
    }
protected:
    QColor getColor(double x, double y)
    {
        /*Vector3 eye = {0.0, 0.0, 0.0};
        Vector3 look;

        look.x = cos(M_PI * (x / 128.0 + 0.5)) * cos(M_PI * (y / 256.0));
        look.y = -sin(M_PI * (y / 256.0));
        look.z = sin(M_PI * (x / 128.0 + 0.5)) * cos(M_PI * (y / 256.0));

        return colorToQColor(skyGetColorCustom(eye, look, &_definition, NULL, NULL));*/
    }
private:
    NoiseGenerator* _noise;
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
    QSlider* slider;
    QPushButton* button;
    
    _base = value;
    _current = noiseCreateGenerator();

    noiseCopy(_base, _current);

    setLayout(new QHBoxLayout());

    previews = new QWidget(this);
    previews->setLayout(new QVBoxLayout());
    layout()->addWidget(previews);
    
    previewLevel = new PreviewLevel(previews, _current);
    previews->layout()->addWidget(new QLabel("Level preview"));
    previews->layout()->addWidget(previewLevel);
    previewTotal = new PreviewTotal(previews, _current);
    previews->layout()->addWidget(new QLabel("Total preview"));
    previews->layout()->addWidget(previewTotal);

    form = new QWidget(this);
    form->setLayout(new QVBoxLayout());
    layout()->addWidget(form);
    
    form->layout()->addWidget(new QLabel("Layers"));
    levels = new QListWidget(form);
    form->layout()->addWidget(levels);
    
    form->layout()->addWidget(new QLabel("Layer height"));
    slider = new QSlider(form);
    slider->setOrientation(Qt::Horizontal);
    slider->setMinimumWidth(150);
    slider->setMaximumWidth(400);
    slider->setMinimum(0);
    slider->setMaximum(1000);
    slider->setTickInterval(100);
    slider->setTickPosition(QSlider::TicksBelow);
    form->layout()->addWidget(slider);

    form->layout()->addWidget(new QLabel("Layer scaling"));
    slider = new QSlider(form);
    slider->setOrientation(Qt::Horizontal);
    slider->setMinimumWidth(150);
    slider->setMaximumWidth(400);
    slider->setMinimum(0);
    slider->setMaximum(1000);
    slider->setTickInterval(100);
    slider->setTickPosition(QSlider::TicksBelow);
    form->layout()->addWidget(slider);
    
    buttons = new QWidget(form);
    buttons->setLayout(new QHBoxLayout());
    form->layout()->addWidget(buttons);
    
    button = new QPushButton("Validate", buttons);
    buttons->layout()->addWidget(button);

    button = new QPushButton("Reset", buttons);
    buttons->layout()->addWidget(button);

    button = new QPushButton("Cancel", buttons);
    buttons->layout()->addWidget(button);

    setWindowTitle("Paysages 3D - Noise editor");
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
