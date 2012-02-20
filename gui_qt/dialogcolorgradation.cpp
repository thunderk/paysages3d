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

/**************** Dialog form ****************/
class FormColorGradation:public BaseForm
{
public:
    FormColorGradation(QWidget* parent, ColorGradation* gradation):BaseForm(parent, true, true)
    {
        _gradation = gradation;
        
        addInputDouble("Position", &_layer.start, 0.0, 1.0, 0.01, 0.1);
        addInputColor("Color", &_layer.col);
        
        setLayerCount(gradation->nbparts);
    }
    
    ~FormColorGradation()
    {
    }
    
    void layerAddedEvent()
    {
        /*if (cloudsAddLayer(&_definition) >= 0)
        {
            BaseForm::layerAddedEvent();
        }*/
    }

    void layerDeletedEvent(int layer)
    {
        /*cloudsDeleteLayer(&_definition, layer);

        BaseForm::layerDeletedEvent(layer);*/
    }

    void layerSelectedEvent(int layer)
    {
        _layer = _gradation->parts[layer];

        BaseForm::layerSelectedEvent(layer);
    }

private:
    ColorGradation* _gradation;
    ColorGradationPart _layer;
};

/**************** Dialog ****************/
DialogColorGradation::DialogColorGradation(QWidget *parent, ColorGradation* gradation):
    QDialog(parent)
{
    QWidget* preview;
    BaseForm* form;
            
    _base = gradation;
    _current = colorGradationCreate();

    setLayout(new QHBoxLayout());
    
    preview = new QWidget(this);
    layout()->addWidget(preview);
    
    form = new FormColorGradation(this, &_current);
    layout()->addWidget(form);
    
    /*QObject::connect(button, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(revert()));
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(reject()));*/

    setWindowTitle("Paysages 3D - Color gradation editor");

    revert();
}

DialogColorGradation::~DialogColorGradation()
{
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
    *_base = _current;
    QDialog::accept();
}

void DialogColorGradation::revert()
{
    _current = *_base;

    revertToCurrent();
}

void DialogColorGradation::revertToCurrent()
{
}
