#include "inputnoise.h"

#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QColorDialog>

#include "dialognoise.h"

class NoiseSmallPreview:public QWidget
{
public:
    NoiseSmallPreview(QWidget* parent, NoiseGenerator* noise):
        QWidget(parent),
        noise(noise)
    {
    }

    void paintEvent(QPaintEvent* event)
    {
        if (!noise)
        {
            return;
        }

        QPainter painter(this);
        int width = this->width();
        int height = this->height();
        double value, factor;

        for (int x = 0; x < width; x++)
        {
            factor = ((double)(height / 2)) / noiseGetMaxValue(noise);
            value = noiseGet1DTotal(noise, ((double)x) / factor) * factor;
            painter.setPen(QColor(255, 255, 255));
            painter.drawLine(x, 0, x, height / 2 + value);
            painter.setPen(QColor(0, 0, 0));
            painter.drawLine(x, height / 2 + value + 1, x, height);
        }
    }
    NoiseGenerator* noise;
};

InputNoise::InputNoise(QWidget* form, QString label, NoiseGenerator* value):
    BaseInput(form, label),
    _value(value)
{
    _preview = new NoiseSmallPreview(form, value);
    _preview->setMinimumSize(100, 40);
    _control = new QPushButton(tr("Edit"), form);
    _control->setMaximumWidth(150);

    connect((QPushButton*)_control, SIGNAL(clicked()), this, SLOT(editNoise()));
}

void InputNoise::updatePreview()
{
    _preview->update();

    BaseInput::updatePreview();
}

void InputNoise::applyValue()
{
    BaseInput::applyValue();
}

void InputNoise::revert()
{
    BaseInput::revert();
}

void InputNoise::editNoise()
{
    if (DialogNoise::getNoise(_control, _value))
    {
        applyValue();
    }
}
