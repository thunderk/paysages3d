#include "baseform.h"

#include "inputdouble.h"
#include "inputint.h"
#include "inputcolor.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>

BaseForm::BaseForm(QWidget* parent) :
    QWidget(parent)
{
    QPushButton* button;
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

    buttons = new QWidget(this);
    buttons->setLayout(new QHBoxLayout());

    hlayout->addWidget(previews);
    hlayout->addWidget(form);
    vlayout->addWidget(hwidget);
    vlayout->addWidget(buttons);

    hwidget->setLayout(hlayout);
    this->setLayout(vlayout);

    button = addButton("Apply");
    connect(button, SIGNAL(clicked()), this, SLOT(applyConfig()));
    button = addButton("Revert");
    connect(button, SIGNAL(clicked()), this, SLOT(revertConfig()));
}

void BaseForm::applyConfigPreview()
{
    QList<Preview*> list_previews = previews->findChildren<Preview*>("_form_preview_");
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

    BaseForm::applyConfigPreview();
}

void BaseForm::applyConfig()
{
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

    connect(input, SIGNAL(valueChanged()), this, SLOT(applyConfigPreview()));

    input->setObjectName("_form_input_");
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
