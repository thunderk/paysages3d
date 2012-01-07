#include "inputnoise.h"

#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QColorDialog>

class NoiseSmallPreview:public QWidget
{
public:
    NoiseSmallPreview(QWidget* parent):
        QWidget(parent)
    {
    }

    void paintEvent(QPaintEvent* event)
    {
        /*QPainter painter(this);
        painter.fillRect(this->rect(), col);*/
    }
    NoiseGenerator* noise;
};

InputNoise::InputNoise(QWidget* form, QString label, NoiseGenerator* value):
    BaseInput(form, label),
    _value(value)
{
    _preview = new NoiseSmallPreview(form);
    _preview->setMinimumSize(100, 40);
    _control = new QPushButton("Edit", form);
    _control->setMaximumWidth(150);

    connect((QPushButton*)_control, SIGNAL(clicked()), this, SLOT(editNoise()));
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
    /*((ColorPreview*)_preview)->col = QColor::fromRgbF(_value->r, _value->g, _value->b);
    _preview->update();*/
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
