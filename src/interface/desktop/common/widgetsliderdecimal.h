#ifndef WIDGETSLIDERDECIMAL_H
#define WIDGETSLIDERDECIMAL_H

#include <QSlider>

class WidgetSliderDecimal : public QSlider
{
    Q_OBJECT
public:
    explicit WidgetSliderDecimal(QWidget *parent = 0);

    void setDecimalRange(double min, double max, double precision=0.0, double tick_interval=0.0);
    void setDecimalValue(double value);
    inline double decimalValue() {return _value;}
    
signals:
    void decimalValueChanged(double value);
    
public slots:
    void processIntValue(int value);
    
private:
    double _min;
    double _max;
    double _step;
    double _value;
};

#endif // WIDGETSLIDERDECIMAL_H
