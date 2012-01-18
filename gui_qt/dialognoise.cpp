#include "dialognoise.h"

#include <QVBoxLayout>
#include <QImage>
#include <QLabel>
#include <QColor>
#include <QPainter>
#include <QScrollArea>

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
    _base = value;
    _current = noiseCreateGenerator();

    noiseCopy(_base, _current);

    setLayout(new QVBoxLayout());

    previewLevel = new PreviewLevel(this, _current);
    layout()->addWidget(new QLabel("Level preview"));
    layout()->addWidget(previewLevel);
    previewTotal = new PreviewTotal(this, _current);
    layout()->addWidget(new QLabel("Total preview"));
    layout()->addWidget(previewTotal);

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
