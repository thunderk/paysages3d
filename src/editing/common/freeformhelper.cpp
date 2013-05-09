#include "freeformhelper.h"

#include <QDialog>
#include <QVariant>
#include <cmath>

Q_DECLARE_METATYPE(double*)

FreeFormHelper::FreeFormHelper(QWidget* form_widget)
    :QObject()
{
    _form_widget = form_widget;
    _data_changed = false;

    _button_apply = NULL;
    _button_revert = NULL;
}

FreeFormHelper::~FreeFormHelper()
{
}

void FreeFormHelper::startManaging()
{
    connect(this, SIGNAL(needLocalRefreshing()), _form_widget, SLOT(refreshFromLocalData()));
    connect(this, SIGNAL(needGlobalRefreshing()), _form_widget, SLOT(refreshFromFellowData()));
    connect(this, SIGNAL(needReverting()), _form_widget, SLOT(updateLocalDataFromScenery()));
    connect(this, SIGNAL(needCommitting()), _form_widget, SLOT(commitLocalDataToScenery()));

    emit needLocalRefreshing();
    emit needGlobalRefreshing();
    emit needReverting();
}

void FreeFormHelper::addPreview(BasePreview* preview)
{
    if (preview && preview->inherits("BasePreview"))
    {
        _previews.append(preview);
    }
}

void FreeFormHelper::addPreview(QString widget_name)
{
    addPreview(_form_widget->findChild<BasePreview*>(widget_name));
}

void FreeFormHelper::addDoubleInputSlider(WidgetSliderDecimal* slider, double* value, double min, double max, double small_step, double large_step)
{
    if (slider && slider->inherits("WidgetSliderDecimal"))
    {
        slider->setDecimalRange(min, max, small_step, large_step);
        slider->setDecimalValue(*value);

        slider->setProperty("data_pointer", QVariant::fromValue<double*>(value));

        connect(slider, SIGNAL(decimalValueChanged(double)), this, SLOT(processDecimalChange(double)));
    }
}

void FreeFormHelper::addDoubleInputSlider(QString widget_name, double* value, double min, double max, double small_step, double large_step)
{
    addDoubleInputSlider(_form_widget->findChild<WidgetSliderDecimal*>(widget_name), value, min, max, small_step, large_step);
}

void FreeFormHelper::setApplyButton(QPushButton* button)
{
    if (button && button->inherits("QPushButton"))
    {
        _button_apply = button;
        button->setEnabled(_data_changed);

        connect(button, SIGNAL(clicked()), this, SLOT(processApplyClicked()));
    }
}

void FreeFormHelper::setApplyButton(QString widget_name)
{
    setApplyButton(_form_widget->findChild<QPushButton*>(widget_name));
}

void FreeFormHelper::setRevertButton(QPushButton* button)
{
    if (button && button->inherits("QPushButton"))
    {
        _button_revert = button;
        button->setEnabled(_data_changed);

        connect(button, SIGNAL(clicked()), this, SLOT(processRevertClicked()));
    }
}

void FreeFormHelper::setRevertButton(QString widget_name)
{
    setRevertButton(_form_widget->findChild<QPushButton*>(widget_name));
}

void FreeFormHelper::setLabelText(QLabel* label, QString text)
{
    if (label && label->inherits("QLabel"))
    {
        label->setText(text);
    }
}

void FreeFormHelper::setLabelText(QString widget_name, QString text)
{
    setLabelText(_form_widget->findChild<QLabel*>(widget_name), text);
}

void FreeFormHelper::openDialog(QDialog* dialog)
{
    if (dialog->exec())
    {
        processDataChange();
    }
}

void FreeFormHelper::processDataChange()
{
    _data_changed = true;
    if (_button_apply)
    {
        _button_apply->setEnabled(true);
    }
    if (_button_revert)
    {
        _button_revert->setEnabled(true);
    }

    for (int i = 0; i < _previews.size(); i++)
    {
        _previews[i]->redraw();
    }

    emit needLocalRefreshing();
}

void FreeFormHelper::processRevertClicked()
{
    emit needReverting();

    _data_changed = false;
    if (_button_apply)
    {
        _button_apply->setEnabled(false);
    }
    if (_button_revert)
    {
        _button_revert->setEnabled(false);
    }

    for (int i = 0; i < _previews.size(); i++)
    {
        _previews[i]->redraw();
    }

    emit needLocalRefreshing();
}

void FreeFormHelper::processApplyClicked()
{
    emit needCommitting();

    _data_changed = false;
    if (_button_apply)
    {
        _button_apply->setEnabled(false);
    }
    if (_button_revert)
    {
        _button_revert->setEnabled(false);
    }
}

void FreeFormHelper::processDecimalChange(double value)
{
    QObject* signal_sender = sender();

    if (signal_sender && signal_sender->inherits("WidgetSliderDecimal"))
    {
        WidgetSliderDecimal* slider = (WidgetSliderDecimal*)signal_sender;
        double* pointer = slider->property("data_pointer").value<double*>();

        if (pointer)
        {
            *pointer = value;
        }

        processDataChange();
    }
}
