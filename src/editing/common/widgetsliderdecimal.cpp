#include "widgetsliderdecimal.h"

#include <cmath>

WidgetSliderDecimal::WidgetSliderDecimal(QWidget *parent) :
    QSlider(parent)
{
    _min = 0.0;
    _max = 1.0;
    _step = 0.01;
    _value = 0.0;

    connect(this, SIGNAL(valueChanged(int)), this, SLOT(processIntValue(int)));
}

void WidgetSliderDecimal::setDecimalRange(double min, double max, double precision, double tick_precision)
{
    double range = max - min;
    if (range < 0.000001)
    {
        max = min + 0.000001;
        range = max - min;
    }
    if (precision < 0.000001)
    {
        precision = range / 1000.0;
    }

    _min = min;
    _max = max;
    _step = precision;

    setMinimum(0);
    setMaximum(round(range / precision));

    if (tick_precision < 0.000001)
    {
        tick_precision = 20.0 * range / precision;
    }
    setTickInterval(round(tick_precision / precision));
}

void WidgetSliderDecimal::setDecimalValue(double value)
{
    _value = value;
    setValue(round((value - _min) / _step));
}

void WidgetSliderDecimal::processIntValue(int value)
{
    double result;
    if (value == maximum())
    {
        result = _max;
    }
    else
    {
        result = _min + ((double)value) * _step;
    }
    if (fabs(result) < 0.0000001)
    {
        result = 0.0;
    }
    _value = result;

    emit decimalValueChanged(result);
}
