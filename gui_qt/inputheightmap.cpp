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
        double min, max, value, fx, fy;
        int ivalue;
        QPainter painter(this);
        
        heightmapGetLimits(_value, &min, &max);
        if (max - min < 0.000001)
        {
            painter.fillRect(rect(), Qt::black);
            return;
        }
        
        fx = 1.0 / (double)(width() - 1);
        fy = 1.0 / (double)(height() - 1);
        for (int x = 0; x < width(); x++)
        {
            for (int y = 0; y < height(); y++)
            {
                value = heightmapGetRawValue(_value, fx * x, fy * y);
                ivalue = (int)(255.0 * (value - min) / (max - min));
                if (ivalue > 255 || ivalue < 0)
                {
                    ivalue = 128;
                }
                painter.setPen(QColor(ivalue, ivalue, ivalue));
                painter.drawPoint(x, y);
            }
        }
    }
    HeightMap* _value;
};

InputHeightMap::InputHeightMap(QWidget* form, QString label, HeightMap* value, TerrainCanvas* canvas) : BaseInput(form, label)
{
    _value = value;
    _canvas = canvas;
    
    _preview = new SmallPreviewHeightMap(form, value);
    _preview->setMinimumSize(100, 100);
    
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
    if (DialogHeightMap::editHeightMap(_control, _value, _canvas))
    {
        applyValue();
    }
}
