#ifndef FORMWATER_H
#define FORMWATER_H

#include <QWidget>

namespace Ui {
    class FormWater;
}

class FormWater : public QWidget
{
    Q_OBJECT

public:
    explicit FormWater(QWidget *parent = 0);
    ~FormWater();

private:
    Ui::FormWater *ui;
};

#endif // FORMWATER_H
