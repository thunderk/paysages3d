#include "dialogcolorgradation.h"

#include <QVBoxLayout>
#include <QImage>
#include <QLabel>
#include <QColor>
#include <QPainter>
#include <QSlider>
#include <QScrollArea>
#include <QPushButton>
#include "baseform.h"
#include "tools.h"
#include "widgetcurveeditor.h"

/**************** Dialog ****************/
DialogColorGradation::DialogColorGradation(QWidget *parent, ColorGradation* gradation):
    QDialog(parent)
{
    _base = gradation;
    _current = colorGradationCreate();
    colorGradationCopy(_base, _current);

    setLayout(new QVBoxLayout());
    
    _curve_editor = new WidgetCurveEditor(this);
    layout()->addWidget(_curve_editor);
    
    _preview_red = new PreviewColorGradation(this, gradation, COLORGRADATIONBAND_RED);
    _preview_red->setMinimumHeight(50);
    connect(_preview_red, SIGNAL(clicked()), this, SLOT(selectRed()));
    layout()->addWidget(_preview_red);

    _preview_green = new PreviewColorGradation(this, gradation, COLORGRADATIONBAND_GREEN);
    _preview_green->setMinimumHeight(50);
    connect(_preview_green, SIGNAL(clicked()), this, SLOT(selectGreen()));
    layout()->addWidget(_preview_green);

    _preview_blue = new PreviewColorGradation(this, gradation, COLORGRADATIONBAND_BLUE);
    _preview_blue->setMinimumHeight(50);
    connect(_preview_blue, SIGNAL(clicked()), this, SLOT(selectBlue()));
    layout()->addWidget(_preview_blue);

    _preview_final = new PreviewColorGradation(this, gradation, COLORGRADATIONBAND_FINAL);
    _preview_final->setMinimumHeight(50);
    layout()->addWidget(_preview_final);
    
    _curve = curveCreate();
    
    /*QObject::connect(button, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(revert()));
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(reject()));*/

    setWindowTitle(tr("Paysages 3D - Color gradation editor"));

    revert();
}

DialogColorGradation::~DialogColorGradation()
{
    colorGradationDelete(_current);
    curveDelete(_curve);
}

bool DialogColorGradation::getGradation(QWidget* parent, ColorGradation* gradation)
{
    int result;

    DialogColorGradation* dialog = new DialogColorGradation(parent, gradation);
    result = dialog->exec();

    delete dialog;

    return (result != 0) ? true : false;
}

void DialogColorGradation::closeEvent(QCloseEvent* e)
{
    reject();
}

void DialogColorGradation::accept()
{
    colorGradationCopy(_current, _base);
    QDialog::accept();
}

void DialogColorGradation::revert()
{
    colorGradationCopy(_base, _current);
    revertToCurrent();
}

void DialogColorGradation::selectRed()
{
    colorGradationGetRedCurve(_current, _curve);
    _curve_editor->setCurve(_curve);
}

void DialogColorGradation::selectGreen()
{
    colorGradationGetGreenCurve(_current, _curve);
    _curve_editor->setCurve(_curve);
}

void DialogColorGradation::selectBlue()
{
    colorGradationGetBlueCurve(_current, _curve);
    _curve_editor->setCurve(_curve);
}

void DialogColorGradation::revertToCurrent()
{
    selectRed();
}
