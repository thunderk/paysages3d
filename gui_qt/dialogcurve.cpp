#include "dialogcurve.h"

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
#include "baseform.h"
#include "tools.h"
#include "widgetcurveeditor.h"

/**************** Dialog ****************/
DialogCurve::DialogCurve(QWidget *parent, Curve* curve, double xmin, double xmax, double ymin, double ymax, QString xlabel, QString ylabel) : QDialog(parent)
{
    QWidget* buttons;
    QWidget* form;
    QGridLayout* form_layout;
    QLabel* label;
    
    _base = curve;
    _current = curveCreate();
    curveCopy(_base, _current);

    setLayout(new QVBoxLayout());
    
    form = new QWidget(this);
    form_layout = new QGridLayout();
    form->setLayout(form_layout);
    layout()->addWidget(form);
    
    label = new QLabel(tr("This is the curve editor.\nClick on points and drag them to move them.\nDouble click to add a new point.\nRight click on a point to delete it."), form);
    label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    label->setMinimumWidth(150);
    label->setMaximumWidth(200);
    label->setWordWrap(true);
    form_layout->addWidget(label, 0, 1);
    _curve_editor = new WidgetCurveEditor(form, xmin, xmax, ymin, ymax);
    _curve_editor->setAxisLabels(xlabel, ylabel);
    _curve_editor->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    form_layout->addWidget(_curve_editor, 0, 0);

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

    setWindowTitle(tr("Paysages 3D - Curve editor"));
    resize(900, 600);

    revert();
}

DialogCurve::~DialogCurve()
{
    curveDelete(_current);
}

bool DialogCurve::getCurve(QWidget* parent, Curve* curve, double xmin, double xmax, double ymin, double ymax, QString xlabel, QString ylabel)
{
    int result;

    DialogCurve* dialog = new DialogCurve(parent, curve, xmin, xmax, ymin, ymax, xlabel, ylabel);
    result = dialog->exec();

    delete dialog;

    return (result != 0) ? true : false;
}

void DialogCurve::closeEvent(QCloseEvent*)
{
    reject();
}

void DialogCurve::accept()
{
    _curve_editor->getCurve(_current);
    curveCopy(_current, _base);
    QDialog::accept();
}

void DialogCurve::revert()
{
    curveCopy(_base, _current);
    revertToCurrent();
}

void DialogCurve::revertToCurrent()
{
    _curve_editor->setCurve(_current);
}
