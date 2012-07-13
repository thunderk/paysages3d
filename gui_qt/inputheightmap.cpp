#include "inputheightmap.h"

#include <QPushButton>
#include <QPainter>
#include "dialogheightmap.h"

class SmallPreviewHeightMap:public QWidget
{
public:
    SmallPreviewHeightMap(QWidget* parent, HeightMap* value) : QWidget(parent)
    {
        _value = value;
    }

    void paintEvent(QPaintEvent* event)
    {
        QPainter painter(this);
        painter.fillRect(this->rect(), Qt::black);
    }
    HeightMap* _value;
};

InputHeightMap::InputHeightMap(QWidget* form, QString label, HeightMap* value) : BaseInput(form, label)
{
    _value = value;
    
    _preview = new SmallPreviewHeightMap(form, value);
    _preview->setMinimumSize(100, 40);
    
    _control = new QPushButton(tr("Paint"), form);
    _control->setMaximumWidth(150);

    connect((QPushButton*)_control, SIGNAL(clicked()), this, SLOT(editHeightMap()));
}

void InputHeightMap::updatePreview()
{
    _preview->update();

    BaseInput::updatePreview();
}

void InputHeightMap::applyValue()
{
    BaseInput::applyValue();
}

void InputHeightMap::revert()
{
    BaseInput::revert();
}

void InputHeightMap::editHeightMap()
{
    if (DialogHeightMap::editHeightMap(_control, _value))
    {
        applyValue();
    }
}
