#ifndef FORMWATER_H
#define FORMWATER_H

#include <QWidget>
#include "preview.h"

namespace Ui {
    class FormWater;
}

class FormWater : public QWidget
{
    Q_OBJECT

public:
    explicit FormWater(QWidget *parent = 0);
    ~FormWater();

    void dataUpdated();

public slots:
    void configChange();
    void applyConfig();
    void revertConfig();

private:
    Ui::FormWater *ui;
    Preview* previewCoverage;
    Preview* previewColor;
};

#endif // FORMWATER_H
