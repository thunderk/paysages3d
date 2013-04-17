#include "dialogcolorgradation.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QImage>
#include <QLabel>
#include <QColor>
#include <QPainter>
#include <QSlider>
#include <QScrollArea>
#include <QPushButton>
#include "tools.h"
#include "widgetcurveeditor.h"

/**************** Dialog ****************/
DialogColorGradation::DialogColorGradation(QWidget *parent, ColorGradation* gradation):
    QDialog(parent)
{
    QWidget* buttons;
    QWidget* form;
    QGridLayout* form_layout;
    QLabel* label;

    _base = gradation;
    _current = colorGradationCreate();
    colorGradationCopy(_base, _current);

    setLayout(new QVBoxLayout());

    form = new QWidget(this);
    form_layout = new QGridLayout();
    form->setLayout(form_layout);
    layout()->addWidget(form);

    label = new QLabel(tr("This is the curve editor for color components.\nClick on a component preview below to edit it.\nClick on points and drag them to move them.\nDouble click to add a new point.\nRight click on a point to delete it."), form);
    label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    label->setMinimumWidth(150);
    label->setMaximumWidth(200);
    label->setWordWrap(true);
    form_layout->addWidget(label, 0, 1);
    _curve_editor = new WidgetCurveEditor(form, 0.0, 1.0, 0.0, 1.0);
    _curve_editor->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    form_layout->addWidget(_curve_editor, 0, 0);
    connect(_curve_editor, SIGNAL(liveChanged()), this, SLOT(updateColors()));

    label = new QLabel(tr("Red preview, click to edit"), form);
    label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    label->setMinimumWidth(150);
    label->setMaximumWidth(200);
    label->setWordWrap(true);
    form_layout->addWidget(label, 1, 1);
    _preview_red = new PreviewColorGradation(form, _current, COLORGRADATIONBAND_RED);
    _preview_red->setMinimumHeight(30);
    _preview_red->setMaximumHeight(60);
    _preview_red->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    connect(_preview_red, SIGNAL(clicked()), this, SLOT(selectRed()));
    form_layout->addWidget(_preview_red, 1, 0);

    label = new QLabel(tr("Green preview, click to edit"), form);
    label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    label->setMinimumWidth(150);
    label->setMaximumWidth(200);
    label->setWordWrap(true);
    form_layout->addWidget(label, 2, 1);
    _preview_green = new PreviewColorGradation(form, _current, COLORGRADATIONBAND_GREEN);
    _preview_green->setMinimumHeight(30);
    _preview_green->setMaximumHeight(60);
    _preview_green->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    connect(_preview_green, SIGNAL(clicked()), this, SLOT(selectGreen()));
    form_layout->addWidget(_preview_green, 2, 0);

    label = new QLabel(tr("Blue preview, click to edit"), form);
    label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    label->setMinimumWidth(150);
    label->setMaximumWidth(200);
    label->setWordWrap(true);
    form_layout->addWidget(label, 3, 1);
    _preview_blue = new PreviewColorGradation(form, _current, COLORGRADATIONBAND_BLUE);
    _preview_blue->setMinimumHeight(30);
    _preview_blue->setMaximumHeight(60);
    _preview_blue->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    connect(_preview_blue, SIGNAL(clicked()), this, SLOT(selectBlue()));
    form_layout->addWidget(_preview_blue, 3, 0);

    label = new QLabel(tr("Final preview"), form);
    label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    label->setMinimumWidth(150);
    label->setMaximumWidth(200);
    label->setWordWrap(true);
    form_layout->addWidget(label, 4, 1);
    _preview_final = new PreviewColorGradation(form, _current, COLORGRADATIONBAND_FINAL);
    _preview_final->setMinimumHeight(30);
    _preview_final->setMaximumHeight(60);
    _preview_final->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    form_layout->addWidget(_preview_final, 4, 0);

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
    resize(900, 600);

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

void DialogColorGradation::closeEvent(QCloseEvent*)
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
    _curve_editor->setPenColor(QColor(255, 0, 0));
    _selected = 1;
}

void DialogColorGradation::selectGreen()
{
    colorGradationGetGreenCurve(_current, _curve);
    _curve_editor->setCurve(_curve);
    _curve_editor->setPenColor(QColor(0, 200, 0));
    _selected = 2;
}

void DialogColorGradation::selectBlue()
{
    colorGradationGetBlueCurve(_current, _curve);
    _curve_editor->setCurve(_curve);
    _curve_editor->setPenColor(QColor(0, 0, 255));
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
