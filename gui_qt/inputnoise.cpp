#include "inputnoise.h"

#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QColorDialog>

#include "../lib_paysages/shared/functions.h"

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
    _control = new QPushButton("Edit", form);
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
    /*_value->r = ((ColorPreview*)_preview)->col.redF();
    _value->g = ((ColorPreview*)_preview)->col.greenF();
    _value->b = ((ColorPreview*)_preview)->col.blueF();
    _value->a = 1.0;*/

    BaseInput::applyValue();
}

void InputNoise::revert()
{
    /*((ColorPreview*)_preview)->col = QColor::fromRgbF(_value->r, _value->g, _value->b);*/

    BaseInput::revert();
}

void InputNoise::editNoise()
{
    /*QColor col = QColorDialog::getColor(((ColorPreview*)_preview)->col, _control);
    if (col.isValid())
    {
        ((ColorPreview*)_preview)->col = col;
        applyValue();
    }*/
}
