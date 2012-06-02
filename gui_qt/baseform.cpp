#include "baseform.h"

#include "inputdouble.h"
#include "inputint.h"
#include "inputboolean.h"
#include "inputcolor.h"
#include "inputcolorgradation.h"
#include "inputnoise.h"
#include "inputcurve.h"
#include "inputmaterial.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

BaseForm::BaseForm(QWidget* parent, bool auto_apply, bool with_layers) : QWidget(parent)
{
    QWidget* control;
    QWidget* layers;
    QLabel* label;

    this->auto_apply = auto_apply;
    this->with_layers = with_layers;

    setLayout(new QHBoxLayout());
    setObjectName("_base_form_");
    
    control = new QWidget(this);
    control->setLayout(new QVBoxLayout());
    control->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    layout()->addWidget(control);
    
    if (with_layers)
    {
        layers = new QWidget(this);
        layers->setLayout(new QHBoxLayout());
        layers->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

        label = new QLabel(tr("Layers : "), layers);
        label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        layers->layout()->addWidget(label);
        
        layer_list = new QComboBox(layers);
        layers->layout()->addWidget(layer_list);
        QObject::connect(layer_list, SIGNAL(currentIndexChanged(int)), this, SLOT(layerListChanged()));

        layer_new = new QPushButton(tr("Add layer"), layers);
        layers->layout()->addWidget(layer_new);
        QObject::connect(layer_new, SIGNAL(clicked()), this, SLOT(layerAddClicked()));
        
        layer_del = new QPushButton(tr("Delete layer"), layers);
        layers->layout()->addWidget(layer_del);
        QObject::connect(layer_del, SIGNAL(clicked()), this, SLOT(layerDelClicked()));
        
        control->layout()->addWidget(layers);
        control->layout()->setAlignment(buttons, Qt::AlignTop);
    }

    previews = new QWidget(this);
    previews->setLayout(new QVBoxLayout());
    previews->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout()->addWidget(previews);
    layout()->setAlignment(previews, Qt::AlignTop);
    
    form = new QWidget(this);
    form->setLayout(new QHBoxLayout());
    control->layout()->addWidget(form);
    control->layout()->setAlignment(form, Qt::AlignTop);
    
    form_labels = new QWidget(form);
    form_labels->setLayout(new QVBoxLayout());
    form->layout()->addWidget(form_labels);

    form_previews = new QWidget(form);
    form_previews->setLayout(new QVBoxLayout());
    form->layout()->addWidget(form_previews);

    form_controls = new QWidget(form);
    form_controls->setLayout(new QVBoxLayout());
    form->layout()->addWidget(form_controls);
    
    buttons = new QWidget(this);
    buttons->setLayout(new QHBoxLayout());
    control->layout()->addWidget(buttons);
    control->layout()->setAlignment(buttons, Qt::AlignBottom);

    button_apply = addButton(tr("Apply"));
    button_apply->setEnabled(false);
    connect(button_apply, SIGNAL(clicked()), this, SLOT(applyConfig()));
    button_revert = addButton(tr("Revert"));
    button_revert->setEnabled(false);
    connect(button_revert, SIGNAL(clicked()), this, SLOT(revertConfig()));

    if (auto_apply)
    {
        hideButtons();
    }
}

void BaseForm::hideButtons()
{
    button_apply->hide();
    button_revert->hide();
}

void BaseForm::configChangeEvent()
{
    if (auto_apply)
    {
        applyConfig();
    }
    else
    {
        button_apply->setEnabled(true);
        button_revert->setEnabled(true);
    }

    QList<BasePreview*> list_previews = previews->findChildren<BasePreview*>("_form_preview_");
    for (int i = 0; i < list_previews.size(); i++)
    {
        list_previews[i]->redraw();
    }
}

void BaseForm::revertConfig()
{
    QList<BaseInput*> inputs = form->findChildren<BaseInput*>("_form_input_");
    for (int i = 0; i < inputs.size(); i++)
    {
        inputs[i]->revert();
    }
    
    if (with_layers)
    {
        if (layer_list->currentIndex() < 0 && layer_list->count() > 0)
        {
            layer_list->setCurrentIndex(0);
        }
    }

    BaseForm::configChangeEvent();

    button_apply->setEnabled(false);
    button_revert->setEnabled(false);
}

void BaseForm::applyConfig()
{
    button_apply->setEnabled(false);
    button_revert->setEnabled(false);

    emit(configApplied());
}

void BaseForm::layerAddClicked()
{
    layerAddedEvent();
}

void BaseForm::layerDelClicked()
{
    layerDeletedEvent(layer_list->currentIndex());
}

void BaseForm::layerListChanged()
{
    layerSelectedEvent(layer_list->currentIndex());
}

void BaseForm::addPreview(BasePreview* preview, QString label)
{
    QLabel* label_widget;
    
    label_widget = new QLabel(label, previews);
    label_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    
    previews->layout()->addWidget(label_widget);
    previews->layout()->addWidget(preview);
    
    preview->setObjectName("_form_preview_");
}

QPushButton* BaseForm::addButton(QString label)
{
    QPushButton* button = new QPushButton(label);
    buttons->layout()->addWidget(button);
    return button;
}

void BaseForm::addInput(BaseInput* input)
{
    int row_height = 30;
    
    form_labels->layout()->addWidget(input->label());
    form_previews->layout()->addWidget(input->preview());
    form_controls->layout()->addWidget(input->control());
    
    input->label()->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    input->label()->setMinimumSize(150, row_height);
    input->label()->setMaximumSize(250, row_height);
    
    input->preview()->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    input->preview()->setMinimumSize(100, row_height);
    input->preview()->setMaximumSize(250, row_height);
    
    input->control()->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    input->control()->setMinimumSize(280, row_height);
    input->control()->setMaximumSize(700, row_height);

    connect(input, SIGNAL(valueChanged()), this, SLOT(configChangeEvent()));

    input->setObjectName("_form_input_");
    input->revert();
}

void BaseForm::addInputInt(QString label, int* value, int min, int max, int small_step, int large_step)
{
    addInput(new InputInt(form, label, value, min, max, small_step, large_step));
}

void BaseForm::addInputDouble(QString label, double* value, double min, double max, double small_step, double large_step)
{
    addInput(new InputDouble(form, label, value, min, max, small_step, large_step));
}

void BaseForm::addInputBoolean(QString label, int* value)
{
    addInput(new InputBoolean(form, label, value));
}

void BaseForm::addInputColor(QString label, Color* value)
{
    addInput(new InputColor(form, label, value));
}

void BaseForm::addInputColorGradation(QString label, ColorGradation* value)
{
    addInput(new InputColorGradation(form, label, value));
}

void BaseForm::addInputNoise(QString label, NoiseGenerator* value)
{
    addInput(new InputNoise(form, label, value));
}

void BaseForm::addInputCurve(QString label, Curve* value, double xmin, double xmax, double ymin, double ymax)
{
    addInput(new InputCurve(form, label, value, xmin, xmax, ymin, ymax));
}

void BaseForm::addInputMaterial(QString label, SurfaceMaterial* material)
{
    addInput(new InputMaterial(form, label, material));
}

int BaseForm::currentLayer()
{
    if (with_layers)
    {
        return layer_list->currentIndex();
    }
    else
    {
        return -1;
    }
}

void BaseForm::setLayerCount(int layer_count)
{
    int i, selected;
    
    if (with_layers)
    {
        selected = layer_list->currentIndex();
        layer_list->clear();

        for (i = 0; i < layer_count; i++)
        {
            layer_list->addItem(QString(tr("Layer %1")).arg(i + 1));
        }
        if (selected >= 0)
        {
            if (selected > layer_count)
            {
                layer_list->setCurrentIndex(layer_count - 1);
            }
            else
            {
                layer_list->setCurrentIndex(selected);
            }
        }
    }
}
    
void BaseForm::layerAddedEvent()
{
    setLayerCount(layer_list->count() + 1);
    layer_list->setCurrentIndex(layer_list->count() - 1);
}

void BaseForm::layerDeletedEvent(int layer)
{
    layer_list->removeItem(layer);
}

void BaseForm::layerSelectedEvent(int layer)
{
    QList<BaseInput*> inputs = form->findChildren<BaseInput*>("_form_input_");
    for (int i = 0; i < inputs.size(); i++)
    {
        inputs[i]->revert();
    }

    QList<BasePreview*> list_previews = previews->findChildren<BasePreview*>("_form_preview_");
    for (int i = 0; i < list_previews.size(); i++)
    {
        list_previews[i]->redraw();
    }
}
