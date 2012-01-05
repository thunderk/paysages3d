#include "baseform.h"
#include "inputdouble.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>

BaseForm::BaseForm(QWidget* parent) :
    QWidget(parent)
{
    QWidget* hwidget;
    QWidget* buttons;
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

    hlayout->addWidget(previews);
    hlayout->addWidget(form);
    vlayout->addWidget(hwidget);
    vlayout->addWidget(buttons);

    hwidget->setLayout(hlayout);
    this->setLayout(vlayout);
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
    QList<InputDouble*> list_doubles = form->findChildren<InputDouble*>("_form_doubleslider_");
    for (int i = 0; i < list_doubles.size(); i++)
    {
        list_doubles[i]->revert();
    }

    BaseForm::applyConfigPreview();
}

void BaseForm::addPreview(Preview* preview, QString label)
{
    previews->layout()->addWidget(new QLabel(label, previews));
    previews->layout()->addWidget(preview);
    preview->setObjectName("_form_preview_");
}

void BaseForm::addInput(BaseInput* input)
{
    QGridLayout* layout = (QGridLayout*)form->layout();
    int row = layout->rowCount();

    layout->addWidget(input->label(), row, 0);
    layout->addWidget(input->preview(), row, 1);
    layout->addWidget(input->control(), row, 2);

    connect(input, SIGNAL(valueChanged()), this, SLOT(applyConfigPreview()));
}

void BaseForm::addInputDouble(QString label, double* value, double min, double max, double small_step, double large_step)
{
    addInput(new InputDouble(form, label, value, min, max, small_step, large_step));
}
