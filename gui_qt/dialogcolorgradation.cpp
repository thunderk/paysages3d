#include "dialogcolorgradation.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
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
    QWidget* buttons;
    
    _base = gradation;
    _current = colorGradationCreate();
    colorGradationCopy(_base, _current);

    setLayout(new QVBoxLayout());
    
    _curve_editor = new WidgetCurveEditor(this);
    layout()->addWidget(_curve_editor);
    connect(_curve_editor, SIGNAL(liveChanged()), this, SLOT(updateColors()));
    
    _preview_red = new PreviewColorGradation(this, _current, COLORGRADATIONBAND_RED);
    _preview_red->setMinimumHeight(50);
    connect(_preview_red, SIGNAL(clicked()), this, SLOT(selectRed()));
    layout()->addWidget(_preview_red);

    _preview_green = new PreviewColorGradation(this, _current, COLORGRADATIONBAND_GREEN);
    _preview_green->setMinimumHeight(50);
    connect(_preview_green, SIGNAL(clicked()), this, SLOT(selectGreen()));
    layout()->addWidget(_preview_green);

    _preview_blue = new PreviewColorGradation(this, _current, COLORGRADATIONBAND_BLUE);
    _preview_blue->setMinimumHeight(50);
    connect(_preview_blue, SIGNAL(clicked()), this, SLOT(selectBlue()));
    layout()->addWidget(_preview_blue);

    _preview_final = new PreviewColorGradation(this, _current, COLORGRADATIONBAND_FINAL);
    _preview_final->setMinimumHeight(50);
    layout()->addWidget(_preview_final);
    
    buttons = new QWidget(this);
    layout()->addWidget(buttons);
    buttons->setLayout(new QHBoxLayout());
    
    _button_accept = new QPushButton(tr("Validate"), buttons);
    buttons->layout()->addWidget(_button_accept);
    QObject::connect(_button_accept, SIGNAL(clicked()), this, SLOT(accept()));

    _button_revert = new QPushButton(tr("Revert"), buttons);
    buttons->layout()->addWidget(_button_revert);
    QObject::connect(_button_revert, SIGNAL(clicked()), this, SLOT(revert()));

    _button_cancel = new QPushButton(tr("Cancel"), buttons);
    buttons->layout()->addWidget(_button_cancel);
    QObject::connect(_button_cancel, SIGNAL(clicked()), this, SLOT(reject()));

    _curve = curveCreate();

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
    _selected = 0;
    colorGradationCopy(_base, _current);
    revertToCurrent();
}

void DialogColorGradation::selectRed()
{
    colorGradationGetRedCurve(_current, _curve);
    _curve_editor->setCurve(_curve);
    _selected = 1;
}

void DialogColorGradation::selectGreen()
{
    colorGradationGetGreenCurve(_current, _curve);
    _curve_editor->setCurve(_curve);
    _selected = 2;
}

void DialogColorGradation::selectBlue()
{
    colorGradationGetBlueCurve(_current, _curve);
    _curve_editor->setCurve(_curve);
    _selected = 3;
}

void DialogColorGradation::updateColors()
{
    Curve* curve;
    
    curve = curveCreate();
    _curve_editor->getCurve(curve);
    
    switch (_selected)
    {
    case 0:
        _preview_red->update();
        _preview_green->update();
        _preview_blue->update();
        break;
    case 1:
        colorGradationSetRedCurve(_current, curve);
        _preview_red->update();
        break;
    case 2:
        colorGradationSetGreenCurve(_current, curve);
        _preview_green->update();
        break;
    case 3:
        colorGradationSetBlueCurve(_current, curve);
        _preview_blue->update();
        break;
    default:
        ;
    }
    _preview_final->update();
    
    curveDelete(curve);
}

void DialogColorGradation::revertToCurrent()
{
    updateColors();
    selectRed();
}
