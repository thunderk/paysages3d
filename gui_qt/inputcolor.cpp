#include "inputcolor.h"

#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QColorDialog>

class ColorPreview:public QWidget
{
public:
    ColorPreview(QWidget* parent):
        QWidget(parent)
    {
    }

    void paintEvent(QPaintEvent* event)
    {
        QPainter painter(this);
        painter.fillRect(this->rect(), col);
    }
    QColor col;
};

InputColor::InputColor(QWidget* form, QString label, Color* value):
    BaseInput(form, label),
    _value(value)
{
    _preview = new ColorPreview(form);
    _preview->setMinimumSize(50, 20);
    _control = new QPushButton("Edit", form);
    _control->setMaximumWidth(150);

    connect((QPushButton*)_control, SIGNAL(clicked()), this, SLOT(chooseColor()));
}

void InputColor::applyValue()
{
    _value->r = ((ColorPreview*)_preview)->col.redF();
    _value->g = ((ColorPreview*)_preview)->col.greenF();
    _value->b = ((ColorPreview*)_preview)->col.blueF();
    _value->a = 1.0;
    BaseInput::applyValue();
}

void InputColor::revert()
{
    ((ColorPreview*)_preview)->col = QColor::fromRgbF(_value->r, _value->g, _value->b);
    _preview->update();
}

void InputColor::chooseColor()
{
    QColor col = QColorDialog::getColor(((ColorPreview*)_preview)->col, _control);
    if (col.isValid())
    {
        ((ColorPreview*)_preview)->col = col;
        applyValue();
    }
}
