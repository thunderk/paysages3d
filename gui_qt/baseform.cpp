#include "baseform.h"

#include "inputdouble.h"
#include "inputint.h"
#include "inputboolean.h"
#include "inputcolor.h"
#include "inputcolorgradation.h"
#include "inputnoise.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <qt4/QtGui/qboxlayout.h>

BaseForm::BaseForm(QWidget* parent, bool auto_apply, bool with_layers) : QWidget(parent)
{
    QWidget* hwidget;
    QVBoxLayout* vlayout;
    QHBoxLayout* hlayout;

    this->auto_apply = auto_apply;
    this->with_layers = with_layers;

    vlayout = new QVBoxLayout();
    hlayout = new QHBoxLayout();
    
    if (with_layers)
    {
        hwidget = new QWidget(this);
        hwidget->setLayout(new QHBoxLayout());

        hwidget->layout()->addWidget(new QLabel("Layers : ", hwidget));
        
        layer_list = new QComboBox(hwidget);
        hwidget->layout()->addWidget(layer_list);
        QObject::connect(layer_list, SIGNAL(currentIndexChanged(int)), this, SLOT(layerListChanged()));

        layer_new = new QPushButton("Add layer", hwidget);
        hwidget->layout()->addWidget(layer_new);
        QObject::connect(layer_new, SIGNAL(clicked()), this, SLOT(layerAddClicked()));
        
        layer_del = new QPushButton("Delete layer", hwidget);
        hwidget->layout()->addWidget(layer_del);
        QObject::connect(layer_del, SIGNAL(clicked()), this, SLOT(layerDelClicked()));
        
        vlayout->addWidget(hwidget);
    }

    hwidget = new QWidget(this);

    previews = new QWidget(this);
    previews->setLayout(new QVBoxLayout());

    form = new QWidget(this);
    form->setLayout(new QGridLayout());
    form->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    buttons = new QWidget(this);
    buttons->setLayout(new QHBoxLayout());

    hlayout->addWidget(previews);
    hlayout->addWidget(form);
    hlayout->setAlignment(form, Qt::AlignTop);

    vlayout->addWidget(hwidget);
    vlayout->addWidget(buttons);

    hwidget->setLayout(hlayout);
    this->setLayout(vlayout);
    this->setObjectName("_base_form_");

    button_apply = addButton("Apply");
    button_apply->setEnabled(false);
    connect(button_apply, SIGNAL(clicked()), this, SLOT(applyConfig()));
    button_revert = addButton("Revert");
    button_revert->setEnabled(false);
    connect(button_revert, SIGNAL(clicked()), this, SLOT(revertConfig()));

    if (auto_apply)
    {
        button_apply->hide();
        button_revert->hide();
    }
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
    previews->layout()->addWidget(new QLabel(label, previews));
    previews->layout()->addWidget(preview);
    
    preview->start();
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
    QGridLayout* layout = (QGridLayout*)form->layout();
    int row = layout->rowCount();

    layout->addWidget(input->label(), row, 0);
    layout->addWidget(input->preview(), row, 1);
    layout->addWidget(input->control(), row, 2);

    input->label()->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    input->preview()->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    input->control()->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

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
            layer_list->addItem(QString("Layer %1").arg(i + 1));
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
