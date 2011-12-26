#include "preview.h"
#include "formwater.h"
#include "ui_formwater.h"
#include <QColor>

#include "../lib_paysages/water.h"
#include "../lib_paysages/shared/functions.h"

class PreviewCoverage:private Preview
{
public:
    PreviewCoverage(QWidget* parent):
        Preview(parent)
    {
    }
protected:
    QColor getColor(double x, double y)
    {
        double height;

        height = terrainGetHeight(x, y);
        if (height > 0.0)
        {
            height = terrainGetHeightNormalized(x, y);
            return QColor((int)(255.0 * height), (int)(255.0 * height), (int)(255.0 * height));
        }
        else
        {
            return QColor(0, 0, 255);
        }
    }
};

FormWater::FormWater(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormWater)
{
    PreviewCoverage* previewCoverage;

    ui->setupUi(this);

    this->findChild<QToolBox*>("water_configs")->setCurrentIndex(0);

    previewCoverage = new PreviewCoverage(this->findChild<QWidget*>("water_preview_coverage"));
}

FormWater::~FormWater()
{
    delete ui;
}
