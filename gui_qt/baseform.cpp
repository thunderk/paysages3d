#include "baseform.h"

#include "inputdouble.h"
#include "inputint.h"
#include "inputboolean.h"
#include "inputcolor.h"
#include "inputcolorgradation.h"
#include "inputnoise.h"
#include "inputcurve.h"
#include "inputmaterial.h"
#include "inputenum.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

BaseForm::BaseForm(QWidget* parent, bool auto_apply, bool with_layers) : QWidget(parent)
{
    QWidget* control;
    QWidget* layers;
    QLabel* label;

    this->_auto_apply = auto_apply;
    this->_with_layers = with_layers;

    setLayout(new QHBoxLayout());
    setObjectName("_base_form_");
    
    control = new QWidget(this);
    control->setLayout(new QVBoxLayout());
    control->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    layout()->addWidget(control);
    
    _layer_count = 0;
    
    if (with_layers)
    {
        layers = new QWidget(this);
        layers->setLayout(new QHBoxLayout());
        layers->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

        label = new QLabel(tr("Layers : "), layers);
        label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        layers->layout()->addWidget(label);
        
        _layer_list = new QComboBox(layers);
        layers->layout()->addWidget(_layer_list);
        QObject::connect(_layer_list, SIGNAL(currentIndexChanged(int)), this, SLOT(layerListChanged()));

        _layer_new = new QPushButton(QIcon("images/layer_add.png"), "", layers);
        _layer_new->setToolTip(tr("Add layer"));
        _layer_new->setMaximumSize(30, 30);
        layers->layout()->addWidget(_layer_new);
        QObject::connect(_layer_new, SIGNAL(clicked()), this, SLOT(layerAddClicked()));
        
        _layer_del = new QPushButton(QIcon("images/layer_del.png"), "", layers);
        _layer_del->setToolTip(tr("Delete layer"));
        _layer_del->setMaximumSize(30, 30);
        layers->layout()->addWidget(_layer_del);
        QObject::connect(_layer_del, SIGNAL(clicked()), this, SLOT(layerDelClicked()));

        _layer_up = new QPushButton(QIcon("images/layer_up.png"), "", layers);
        _layer_up->setToolTip(tr("Move layer upward"));
        _layer_up->setMaximumSize(30, 30);
        layers->layout()->addWidget(_layer_up);
        QObject::connect(_layer_up, SIGNAL(clicked()), this, SLOT(layerUpClicked()));

        _layer_down = new QPushButton(QIcon("images/layer_down.png"), "", layers);
        _layer_down->setToolTip(tr("Move layer downward"));
        _layer_down->setMaximumSize(30, 30);
        layers->layout()->addWidget(_layer_down);
        QObject::connect(_layer_down, SIGNAL(clicked()), this, SLOT(layerDownClicked()));
        
        control->layout()->addWidget(layers);
        control->layout()->setAlignment(_buttons, Qt::AlignTop);
    }

    _previews = new QWidget(this);
    _previews->setLayout(new QVBoxLayout());
    _previews->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout()->addWidget(_previews);
    layout()->setAlignment(_previews, Qt::AlignTop);
    
    _form = new QWidget(this);
    _form->setLayout(new QHBoxLayout());
    control->layout()->addWidget(_form);
    control->layout()->setAlignment(_form, Qt::AlignTop);
    
    _form_labels = new QWidget(_form);
    _form_labels->setLayout(new QVBoxLayout());
    _form->layout()->addWidget(_form_labels);

    _form_previews = new QWidget(_form);
    _form_previews->setLayout(new QVBoxLayout());
    _form->layout()->addWidget(_form_previews);

    _form_controls = new QWidget(_form);
    _form_controls->setLayout(new QVBoxLayout());
    _form->layout()->addWidget(_form_controls);
    
    _buttons = new QWidget(this);
    _buttons->setLayout(new QHBoxLayout());
    control->layout()->addWidget(_buttons);
    control->layout()->setAlignment(_buttons, Qt::AlignBottom);

    _button_apply = addButton(tr("Apply"));
    _button_apply->setEnabled(false);
    connect(_button_apply, SIGNAL(clicked()), this, SLOT(applyConfig()));
    _button_revert = addButton(tr("Revert"));
    _button_revert->setEnabled(false);
    connect(_button_revert, SIGNAL(clicked()), this, SLOT(revertConfig()));
    
    _auto_update_previews = true;

    if (auto_apply)
    {
        hideButtons();
    }
}

void BaseForm::hideButtons()
{
    _button_apply->hide();
    _button_revert->hide();
}

void BaseForm::savePack(PackStream* stream)
{
    // Save previews status
    // TODO Ensure same order in save and load
    QList<BasePreview*> list_previews = _previews->findChildren<BasePreview*>("_form_preview_");
    for (int i = 0; i < list_previews.size(); i++)
    {
        list_previews[i]->savePack(stream);
    }
}

void BaseForm::loadPack(PackStream* stream)
{
    // Load previews status
    // TODO Ensure same order in save and load
    QList<BasePreview*> list_previews = _previews->findChildren<BasePreview*>("_form_preview_");
    for (int i = 0; i < list_previews.size(); i++)
    {
        list_previews[i]->loadPack(stream);
    }
}

void BaseForm::configChangeEvent()
{
    if (_auto_apply)
    {
        applyConfig();
    }
    else
    {
        _button_apply->setEnabled(true);
        _button_revert->setEnabled(true);
    }

    QList<BaseInput*> inputs = _form->findChildren<BaseInput*>("_form_input_");
    for (int i = 0; i < inputs.size(); i++)
    {
        if (_with_layers && _layer_list->count() == 0)
        {
            inputs[i]->checkVisibility(false);
        }
        else
        {
            inputs[i]->checkVisibility(true);
        }
    }
    
    if (_auto_update_previews)
    {
        updatePreviews();
    }
}

void BaseForm::revertConfig()
{
    QList<BaseInput*> inputs = _form->findChildren<BaseInput*>("_form_input_");
    for (int i = 0; i < inputs.size(); i++)
    {
        inputs[i]->revert();
    }
    
    if (_with_layers)
    {
        if (_layer_list->currentIndex() < 0 && _layer_list->count() > 0)
        {
            _layer_list->setCurrentIndex(0);
        }
    }

    updatePreviews();
    //configChangeEvent();

    _button_apply->setEnabled(false);
    _button_revert->setEnabled(false);
}

void BaseForm::applyConfig()
{
    _button_apply->setEnabled(false);
    _button_revert->setEnabled(false);

    emit(configApplied());
}

void BaseForm::rebuildLayerList()
{
    if (_with_layers)
    {
        int selected = _layer_list->currentIndex();
        _layer_list->clear();

        for (int i = 0; i < _layer_count; i++)
        {
            _layer_list->addItem(QString(tr("Layer %1")).arg(i + 1));
        }
        if (selected >= 0)
        {
            if (selected >= _layer_count)
            {
                _layer_list->setCurrentIndex(_layer_count - 1);
            }
            else
            {
                _layer_list->setCurrentIndex(selected);
            }
        }
    }
}

void BaseForm::layerAddClicked()
{
    layerAddedEvent();
    
    rebuildLayerList();
    _layer_list->setCurrentIndex(_layer_list->count() - 1);
    
    _button_apply->setEnabled(true);
    _button_revert->setEnabled(true);
}

void BaseForm::layerDelClicked()
{
    if (_layer_list->currentIndex() >= 0)
    {
        layerDeletedEvent(_layer_list->currentIndex());

        rebuildLayerList();

        _button_apply->setEnabled(true);
        _button_revert->setEnabled(true);
    }
}
    
void BaseForm::layerUpClicked()
{
    if (_layer_list->currentIndex() < _layer_count - 1)
    {
        layerMovedEvent(_layer_list->currentIndex(), _layer_list->currentIndex() + 1);

        rebuildLayerList();

        _button_apply->setEnabled(true);
        _button_revert->setEnabled(true);
    }
}

void BaseForm::layerDownClicked()
{
    if (_layer_list->currentIndex() > 0)
    {
        layerMovedEvent(_layer_list->currentIndex(), _layer_list->currentIndex() - 1);

        rebuildLayerList();

        _button_apply->setEnabled(true);
        _button_revert->setEnabled(true);
    }
}

void BaseForm::layerListChanged()
{
    bool changed = _button_apply->isEnabled();
    
    layerSelectedEvent(_layer_list->currentIndex());
    
    _button_apply->setEnabled(changed);
    _button_revert->setEnabled(changed);
}

void BaseForm::addPreview(BasePreview* preview, QString label)
{
    QLabel* label_widget;
    
    label_widget = new QLabel(label, _previews);
    label_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    
    _previews->layout()->addWidget(label_widget);
    _previews->layout()->addWidget(preview);
    
    preview->setObjectName("_form_preview_");
}

QPushButton* BaseForm::addButton(QString label)
{
    QPushButton* button = new QPushButton(label);
    _buttons->layout()->addWidget(button);
    return button;
}

BaseInput* BaseForm::addInput(BaseInput* input)
{
    int row_height = 30;
    
    _form_labels->layout()->addWidget(input->label());
    _form_previews->layout()->addWidget(input->preview());
    _form_controls->layout()->addWidget(input->control());
    
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
    
    return input;
}

BaseInput* BaseForm::addInputInt(QString label, int* value, int min, int max, int small_step, int large_step)
{
    return addInput(new InputInt(_form, label, value, min, max, small_step, large_step));
}

BaseInput* BaseForm::addInputDouble(QString label, double* value, double min, double max, double small_step, double large_step)
{
    return addInput(new InputDouble(_form, label, value, min, max, small_step, large_step));
}

BaseInput* BaseForm::addInputBoolean(QString label, int* value)
{
    return addInput(new InputBoolean(_form, label, value));
}

BaseInput* BaseForm::addInputColor(QString label, Color* value)
{
    return addInput(new InputColor(_form, label, value));
}

BaseInput* BaseForm::addInputColorGradation(QString label, ColorGradation* value)
{
    return addInput(new InputColorGradation(_form, label, value));
}

BaseInput* BaseForm::addInputNoise(QString label, NoiseGenerator* value)
{
    return addInput(new InputNoise(_form, label, value));
}

BaseInput* BaseForm::addInputCurve(QString label, Curve* value, double xmin, double xmax, double ymin, double ymax, QString xlabel, QString ylabel)
{
    return addInput(new InputCurve(_form, label, value, xmin, xmax, ymin, ymax, xlabel, ylabel));
}

BaseInput* BaseForm::addInputMaterial(QString label, SurfaceMaterial* material)
{
    return addInput(new InputMaterial(_form, label, material));
}

BaseInput* BaseForm::addInputEnum(QString label, int* value, const QStringList& values)
{
    return addInput(new InputEnum(_form, label, value, values));
}

void BaseForm::updatePreviews()
{
    QList<BasePreview*> list_previews = _previews->findChildren<BasePreview*>("_form_preview_");
    for (int i = 0; i < list_previews.size(); i++)
    {
        list_previews[i]->redraw();
    }
}

void BaseForm::disablePreviewsUpdate()
{
    _auto_update_previews = false;
}

int BaseForm::currentLayer()
{
    if (_with_layers)
    {
        return _layer_list->currentIndex();
    }
    else
    {
        return -1;
    }
}

void BaseForm::setLayerCount(int layer_count)
{
    this->_layer_count = layer_count;
    rebuildLayerList();
}
    
void BaseForm::layerAddedEvent()
{
}

void BaseForm::layerDeletedEvent(int layer)
{
}

void BaseForm::layerMovedEvent(int layer, int new_position)
{
}

void BaseForm::layerSelectedEvent(int layer)
{
    QList<BaseInput*> inputs = _form->findChildren<BaseInput*>("_form_input_");
    for (int i = 0; i < inputs.size(); i++)
    {
        inputs[i]->revert();
        inputs[i]->checkVisibility(layer >= 0);
    }

    QList<BasePreview*> list_previews = _previews->findChildren<BasePreview*>("_form_preview_");
    for (int i = 0; i < list_previews.size(); i++)
    {
        list_previews[i]->redraw();
    }
}
