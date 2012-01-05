#include "baseform.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>

class DoubleSliderWidget:public QWidget
{
public:
    DoubleSliderWidget(QWidget* parent, double* value, double min, double max, double small_step, double large_step):
        QWidget(parent),
        value(value), min(min), max(max), small_step(small_step), large_step(large_step)
    {
        setObjectName("_form_doubleslider_");

        setLayout(new QHBoxLayout());

        slider = new QSlider(this);
        slider->setOrientation(Qt::Horizontal);
        slider->setMinimum(min / small_step);
        slider->setMaximum(max / small_step);
        slider->setValue(*value / small_step);
        slider->setTickInterval(large_step / small_step);
        slider->setTickPosition(QSlider::TicksBelow);

        layout()->addWidget(slider);
    }

    void revert()
    {
        slider->setValue(*value / small_step);
    }

private:
    QSlider* slider;
    double* value;
    double min;
    double max;
    double small_step;
    double large_step;
};

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

void BaseForm::revertConfig()
{
    QList<Preview*> list_previews = previews->findChildren<Preview*>("_form_preview_");
    for (int i = 0; i < list_previews.size(); i++)
    {
        list_previews[i]->redraw();
    }

    QList<DoubleSliderWidget*> list_doubles = form->findChildren<DoubleSliderWidget*>("_form_doubleslider_");
    for (int i = 0; i < list_doubles.size(); i++)
    {
        list_doubles[i]->revert();
    }
}

void BaseForm::addPreview(Preview* preview, QString label)
{
    previews->layout()->addWidget(new QLabel(label, previews));
    previews->layout()->addWidget(preview);
    preview->setObjectName("_form_preview_");
}

void BaseForm::addDoubleSlider(QString label, double* value, double min, double max, double small_step, double large_step)
{
    QGridLayout* layout = (QGridLayout*)form->layout();
    int row = layout->rowCount();

    layout->addWidget(new QLabel(label, form), row, 0);
    layout->addWidget(new DoubleSliderWidget(form, value, min, max, small_step, large_step), row, 1);
}
