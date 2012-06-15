#include "inputcurve.h"

#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include "dialogcurve.h"

class CurveSmallPreview:public QWidget
{
public:
    CurveSmallPreview(QWidget* parent, Curve* curve, float xmin, float xmax, float ymin, float ymax) : QWidget(parent)
    {
        _curve = curve;
        _xmin = xmin;
        _xmax = xmax;
        _ymin = ymin;
        _ymax = ymax;
    }

    void paintEvent(QPaintEvent* event)
    {
        if (!_curve)
        {
            return;
        }

        QPainter painter(this);
        int width = this->width();
        int height = this->height();
        float position, value, prev_value, next_value;

        painter.fillRect(rect(), QColor(255, 255, 255));
        for (int x = 0; x < width; x++)
        {
            painter.setPen(QColor(0, 0, 0));
            position = _xmin + (_xmax - _xmin) * (float)x / (float)(width - 1);
            value = (curveGetValue(_curve, position) - _ymin) * (_ymax - _ymin);
            prev_value = curveGetValue(_curve, position - 1.0 / (float)(width - 1));
            next_value = curveGetValue(_curve, position + 1.0 / (float)(width - 1));

            painter.drawLine(x, height - 1 - (int)((value + (prev_value - value) / 2.0) * (float)(height - 1)), x, height - 1 - (int)((value + (next_value - value) / 2.0) * (float)(height - 1)));
            painter.drawPoint(x, height - 1 - (int)(value * (float)(height - 1)));
        }
    }
    Curve* _curve;
    float _xmin;
    float _xmax;
    float _ymin;
    float _ymax;
};

InputCurve::InputCurve(QWidget* form, QString label, Curve* value, float xmin, float xmax, float ymin, float ymax) : BaseInput(form, label)
{
    _value = value;
    _xmin = xmin;
    _xmax = xmax;
    _ymin = ymin;
    _ymax = ymax;
    
    _preview = new CurveSmallPreview(form, value, xmin, xmax, ymin, ymax);
    _preview->setMinimumSize(100, 40);
    _control = new QPushButton(tr("Edit"), form);
    _control->setMaximumWidth(150);

    connect((QPushButton*)_control, SIGNAL(clicked()), this, SLOT(editCurve()));
}

void InputCurve::updatePreview()
{
    _preview->update();

    BaseInput::updatePreview();
}

void InputCurve::applyValue()
{
    BaseInput::applyValue();
}

void InputCurve::revert()
{
    BaseInput::revert();
}

void InputCurve::editCurve()
{
    if (DialogCurve::getCurve(_control, _value, _xmin, _xmax, _ymin, _ymax))
    {
        applyValue();
    }
}
