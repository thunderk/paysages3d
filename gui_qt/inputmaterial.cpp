#include "inputmaterial.h"

#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QColorDialog>
#include "dialogmaterial.h"

class MaterialSmallPreview:public QWidget
{
public:
    MaterialSmallPreview(QWidget* parent, SurfaceMaterial* value) : QWidget(parent)
    {
        _value = value;
    }

    void paintEvent(QPaintEvent* event)
    {
        if (!_value)
        {
            return;
        }

        /*QPainter painter(this);
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
        }*/
    }
    
    SurfaceMaterial* _value;
};

InputMaterial::InputMaterial(QWidget* form, QString label, SurfaceMaterial* value) : BaseInput(form, label)
{
    _value = value;
    
    _preview = new MaterialSmallPreview(form, value);
    _preview->setMinimumSize(100, 40);
    
    _control = new QPushButton(tr("Edit"), form);
    _control->setMaximumWidth(150);

    connect((QPushButton*)_control, SIGNAL(clicked()), this, SLOT(editMaterial()));
}

void InputMaterial::updatePreview()
{
    _preview->update();

    BaseInput::updatePreview();
}

void InputMaterial::applyValue()
{
    BaseInput::applyValue();
}

void InputMaterial::revert()
{
    BaseInput::revert();
}

void InputMaterial::editMaterial()
{
    if (DialogMaterial::getMaterial(_control, _value))
    {
        applyValue();
    }
}
