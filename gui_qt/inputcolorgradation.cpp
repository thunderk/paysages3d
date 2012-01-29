#include "inputcolorgradation.h"

#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QColorDialog>

#include "tools.h"

class ColorGradationPreview:public QWidget
{
public:
    ColorGradationPreview(QWidget* parent, ColorGradation* gradation):
        QWidget(parent),
        gradation(gradation)
    {
    }

    void paintEvent(QPaintEvent* event)
    {
        QPainter painter(this);
        int width = this->width();
        int height = this->height();

        for (int x = 0; x < width; x++)
        {
            painter.setPen(colorToQColor(colorGradationGet(gradation, (double)x / (double)width)));
            painter.drawLine(x, 0, x, height - 1);
        }
    }

    ColorGradation* gradation;
};

InputColorGradation::InputColorGradation(QWidget* form, QString label, ColorGradation* value):
    BaseInput(form, label),
    _value(value)
{
    _preview = new ColorGradationPreview(form, value);
    _preview->setMinimumSize(200, 20);

    _control = new QPushButton("Edit", form);
    _control->setMaximumWidth(150);

    connect((QPushButton*)_control, SIGNAL(clicked()), this, SLOT(editGradation()));
}

void InputColorGradation::updatePreview()
{
    _preview->update();
    BaseInput::updatePreview();
}

void InputColorGradation::applyValue()
{
    /*_value->r = ((ColorPreview*)_preview)->col.redF();
    _value->g = ((ColorPreview*)_preview)->col.greenF();
    _value->b = ((ColorPreview*)_preview)->col.blueF();
    _value->a = 1.0;*/
    BaseInput::applyValue();
}

void InputColorGradation::revert()
{
    BaseInput::revert();
}

void InputColorGradation::editGradation()
{
    /*QColor col = QColorDialog::getColor(((ColorPreview*)_preview)->col, _control);
    if (col.isValid())
    {
        ((ColorPreview*)_preview)->col = col;
        applyValue();
    }*/
}
