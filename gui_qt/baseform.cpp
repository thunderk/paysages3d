#include "baseform.h"

#include "inputdouble.h"
#include "inputint.h"
#include "inputcolor.h"
#include "inputcolorgradation.h"
#include "inputnoise.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>

BaseForm::BaseForm(QWidget* parent) :
    QWidget(parent)
{
    QWidget* hwidget;
    QVBoxLayout* vlayout;
    QHBoxLayout* hlayout;

    vlayout = new QVBoxLayout();
    hlayout = new QHBoxLayout();

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
}

void BaseForm::configChangeEvent()
{
    QList<Preview*> list_previews = previews->findChildren<Preview*>("_form_preview_");
    for (int i = 0; i < list_previews.size(); i++)
    {
        list_previews[i]->redraw();
    }

    button_apply->setEnabled(true);
    button_revert->setEnabled(true);
}

void BaseForm::revertConfig()
{
    QList<BaseInput*> inputs = form->findChildren<BaseInput*>("_form_input_");
    for (int i = 0; i < inputs.size(); i++)
    {
        inputs[i]->revert();
    }

    BaseForm::configChangeEvent();

    button_apply->setEnabled(false);
    button_revert->setEnabled(false);
}

void BaseForm::applyConfig()
{
    revertConfig();
    emit(configApplied());
}

void BaseForm::addPreview(Preview* preview, QString label)
{
    previews->layout()->addWidget(new QLabel(label, previews));
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
