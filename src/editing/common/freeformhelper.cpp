#include "freeformhelper.h"

#include <QDialog>
#include <QVariant>
#include <cmath>
#include "dialogrender.h"
#include "dialogexplorer.h"
#include "rendering/scenery.h"
#include "rendering/renderer.h"

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
    connect(this, SIGNAL(needAlterRenderer(Renderer*)), _form_widget, SLOT(alterRenderer(Renderer*)));

    emit needLocalRefreshing();
    emit needGlobalRefreshing();
    emit needReverting();
}

void FreeFormHelper::addPreview(BasePreview* preview, PreviewRenderer* renderer)
{
    if (preview && preview->inherits("BasePreview"))
    {
        _previews.append(preview);
        preview->setRenderer(renderer);
    }
}

void FreeFormHelper::addPreview(QString widget_name, PreviewRenderer* renderer)
{
    addPreview(_form_widget->findChild<BasePreview*>(widget_name), renderer);
}

void FreeFormHelper::addDoubleInputSlider(WidgetSliderDecimal* slider, double* value, double min, double max, double small_step, double large_step)
{
    if (slider && slider->inherits("WidgetSliderDecimal"))
    {
        _inputs_decimal.append(slider);

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

void FreeFormHelper::setExploreButton(QPushButton* button)
{
    if (button && button->inherits("QPushButton"))
    {
        _button_explore = button;

        connect(button, SIGNAL(clicked()), this, SLOT(processExploreClicked()));
    }
}

void FreeFormHelper::setExploreButton(QString widget_name)
{
    setExploreButton(_form_widget->findChild<QPushButton*>(widget_name));
}

void FreeFormHelper::setRenderButton(QPushButton* button)
{
    if (button && button->inherits("QPushButton"))
    {
        _button_render = button;

        connect(button, SIGNAL(clicked()), this, SLOT(processRenderClicked()));
    }
}

void FreeFormHelper::setRenderButton(QString widget_name)
{
    setRenderButton(_form_widget->findChild<QPushButton*>(widget_name));
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

    for (int i = 0; i < _previews.size(); i++)
    {
        _previews[i]->redraw();
    }

    for (int i = 0; i < _inputs_decimal.size(); i++)
    {
        WidgetSliderDecimal* slider = _inputs_decimal.at(i);
        double* pointer = slider->property("data_pointer").value<double*>();

        if (pointer)
        {
            slider->setDecimalValue(*pointer);
        }
    }

    _data_changed = false;
    if (_button_apply)
    {
        _button_apply->setEnabled(false);
    }
    if (_button_revert)
    {
        _button_revert->setEnabled(false);
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

void FreeFormHelper::processExploreClicked()
{
    Renderer* renderer;

    renderer = sceneryCreateStandardRenderer();

    emit needAlterRenderer(renderer);

    CameraDefinition* camera = cameraCreateDefinition();
    sceneryGetCamera(camera);

    DialogExplorer* dialog = new DialogExplorer(_form_widget, camera, false, renderer);
    dialog->exec();
    delete dialog;

    rendererDelete(renderer);
    cameraDeleteDefinition(camera);
}

void FreeFormHelper::processRenderClicked()
{
    Renderer* renderer;

    renderer = sceneryCreateStandardRenderer();

    emit needAlterRenderer(renderer);

    DialogRender* dialog = new DialogRender(_form_widget, renderer);
    RenderParams params = {400, 300, 1, 3};
    dialog->startRender(params);

    delete dialog;

    rendererDelete(renderer);
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
