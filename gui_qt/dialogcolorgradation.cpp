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

    setLayout(new QHBoxLayout());
    
    _curve_editor = new WidgetCurveEditor(this);
    layout()->addWidget(_curve_editor);
    
    /*QObject::connect(button, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(revert()));
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(reject()));*/

    setWindowTitle(tr("Paysages 3D - Color gradation editor"));

    revert();
}

DialogColorGradation::~DialogColorGradation()
{
    colorGradationDelete(_current);
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

void DialogColorGradation::revertToCurrent()
{
    // TODO
}
