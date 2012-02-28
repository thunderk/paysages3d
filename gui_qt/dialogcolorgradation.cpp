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

/**************** Preview ****************/
class PreviewColorGradation:public BasePreview
{
public:
    PreviewColorGradation(QWidget* parent, ColorGradation* gradation): BasePreview(parent)
    {
        _gradation_original = gradation;
        _gradation_preview = *gradation;
        setMinimumSize(300, 30);
        setMaximumSize(300, 30);
        resize(300, 30);
        // TODO No scrolling/scaling
    }

protected:
    void updateData()
    {
        _gradation_preview = *_gradation_original;
    }
    QColor getColor(double x, double y)
    {
        return colorToQColor(colorGradationGet(&_gradation_preview, x / 600 + 0.5));
    }
private:
    ColorGradation* _gradation_original;
    ColorGradation _gradation_preview;
};

/**************** Form ****************/
class FormColorGradation:public BaseForm
{
public:
    FormColorGradation(QWidget* parent, ColorGradation* gradation):BaseForm(parent, true, true)
    {
        QPushButton* button;
        _gradation = gradation;
        
        addPreview(new PreviewColorGradation(this, _gradation), tr("Preview"));
        
        addInputDouble(tr("Position"), &_layer.start, 0.0, 1.0, 0.01, 0.1);
        addInputColor(tr("Color"), &_layer.col);
        
        button = addButton(tr("Validate"));
        QObject::connect(button, SIGNAL(clicked()), parent, SLOT(accept()));
        
        button = addButton(tr("Revert"));
        QObject::connect(button, SIGNAL(clicked()), parent, SLOT(revert()));
        
        button = addButton(tr("Cancel"));
        QObject::connect(button, SIGNAL(clicked()), parent, SLOT(reject()));
        
        revertConfig();
    }
    
    void revertConfig()
    {
        setLayerCount(colorGradationGetPartCount(_gradation));
    }
    
    void configChangeEvent()
    {
        colorGradationSetPart(_gradation, currentLayer(), _layer);
        
        BaseForm::configChangeEvent();
    }
    
    void layerAddedEvent()
    {
        if (colorGradationAddPart(_gradation) >= 0)
        {
            BaseForm::layerAddedEvent();
        }
    }

    void layerDeletedEvent(int layer)
    {
        colorGradationDelPart(_gradation, layer);

        BaseForm::layerDeletedEvent(layer);
    }

    void layerSelectedEvent(int layer)
    {
        _layer = colorGradationGetPart(_gradation, layer);

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
            
    _base = gradation;
    _current = colorGradationCreate();
    _current = *_base;

    setLayout(new QHBoxLayout());
    
    preview = new QWidget(this);
    layout()->addWidget(preview);
    
    _form = new FormColorGradation(this, &_current);
    layout()->addWidget(_form);
    
    /*QObject::connect(button, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(revert()));
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(reject()));*/

    setWindowTitle(tr("Paysages 3D - Color gradation editor"));

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
    _form->revertConfig();
}
