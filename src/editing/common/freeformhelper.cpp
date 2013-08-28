#include "freeformhelper.h"

#include <cmath>
#include <QDialog>
#include <QVariant>
#include <QResizeEvent>
#include <QSlider>
#include <QPushButton>
#include <QWidget>
#include <QInputDialog>
#include "mainwindow.h"
#include "dialogrender.h"
#include "dialogexplorer.h"
#include "rendering/scenery.h"
#include "rendering/renderer.h"
#include "tools.h"

Q_DECLARE_METATYPE(double*)

FreeFormHelper::FreeFormHelper(QWidget* form_widget)
    :QObject()
{
    _form_widget = form_widget;
    _data_changed = false;

    _button_apply = NULL;
    _button_revert = NULL;
    _button_presets = NULL;
}

FreeFormHelper::~FreeFormHelper()
{
}

bool FreeFormHelper::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::Resize && object == _form_widget)
    {
        QSize form_size = ((QResizeEvent*) event)->size();
        QSize preview_size;

        if (form_size.width() > 1400 && form_size.height() > 900)
        {
            preview_size = QSize(320, 320);
        }
        else if (form_size.width() < 1100 || form_size.height() < 750)
        {
            preview_size = QSize(200, 200);
        }
        else
        {
            preview_size = QSize(260, 260);
        }

        for (int i = 0; i < _previews.size(); i++)
        {
            if (_previews[i]->size() != preview_size)
            {
                _previews[i]->setMinimumSize(preview_size);
                _previews[i]->setMaximumSize(preview_size);
                _previews[i]->resize(preview_size);
            }
        }
    }
    return false;
}

void FreeFormHelper::startManaging()
{
    _form_widget->installEventFilter(this);
    connect(MainWindow::instance(), SIGNAL(refreshed()), this, SLOT(processGlobalRefresh()));

    connect(this, SIGNAL(needLocalRefreshing()), _form_widget, SLOT(refreshFromLocalData()));
    connect(this, SIGNAL(needGlobalRefreshing()), _form_widget, SLOT(refreshFromFellowData()));
    connect(this, SIGNAL(needReverting()), _form_widget, SLOT(updateLocalDataFromScenery()));
    connect(this, SIGNAL(needCommitting()), _form_widget, SLOT(commitLocalDataToScenery()));
    connect(this, SIGNAL(needAlterRenderer(Renderer*)), _form_widget, SLOT(alterRenderer(Renderer*)));

    processGlobalRefresh();
}

void FreeFormHelper::addPreview(BasePreview* preview, PreviewRenderer* renderer)
{
    _previews.append(preview);
    preview->setRenderer(renderer);
}

void FreeFormHelper::addDoubleInputSlider(WidgetSliderDecimal* slider, double* value, double min, double max, double small_step, double large_step)
{
    _inputs_decimal.append(slider);

    slider->setDecimalRange(min, max, small_step, large_step);
    slider->setDecimalValue(*value);

    slider->setProperty("data_pointer", QVariant::fromValue<double*>(value));

    connect(slider, SIGNAL(decimalValueChanged(double)), this, SLOT(processDecimalChange(double)));
}

void FreeFormHelper::setApplyButton(QPushButton* button)
{
    _button_apply = button;
    button->setEnabled(_data_changed);

    connect(button, SIGNAL(clicked()), this, SLOT(processApplyClicked()));
}

void FreeFormHelper::setRevertButton(QPushButton* button)
{
    _button_revert = button;
    button->setEnabled(_data_changed);

    connect(button, SIGNAL(clicked()), this, SLOT(processRevertClicked()));
}

void FreeFormHelper::setExploreButton(QPushButton* button)
{
    _button_explore = button;

    button->setToolTip(tr("Explore the scenery in 3D, with current changes applied"));

    connect(button, SIGNAL(clicked()), this, SLOT(processExploreClicked()));
}

void FreeFormHelper::setRenderButton(QPushButton* button)
{
    _button_render = button;

    button->setToolTip(tr("Quick render preview, with current changes applied"));

    connect(button, SIGNAL(clicked()), this, SLOT(processRenderClicked()));
}

void FreeFormHelper::setLabelText(QLabel* label, QString text)
{
    label->setText(text);
}

void FreeFormHelper::addPreset(const QString& name)
{
    _presets << name;
}

void FreeFormHelper::setPresetButton(QPushButton* button)
{
    _button_presets = button;
    connect(button, SIGNAL(clicked()), this, SLOT(processPresetClicked()));
}

void FreeFormHelper::openDialog(QDialog* dialog)
{
    if (dialog->exec())
    {
        processDataChange();
    }
}

void FreeFormHelper::gotoMainTab(int position)
{
    MainWindow::instance()->openTab(position);
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

void FreeFormHelper::processGlobalRefresh()
{
    emit needGlobalRefreshing();

    processRevertClicked();
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

    MainWindow::instance()->refreshAll();
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

void FreeFormHelper::processPresetClicked()
{
    bool ok;
    QString item = QInputDialog::getItem(_form_widget, tr("Choose a preset"), tr("Preset settings : "), _presets, 0, false, &ok);

    if (ok && !item.isEmpty())
    {
        int preset = _presets.indexOf(item);
        if (preset >= 0)
        {
            emit presetSelected(preset);
            processDataChange();
        }
    }
}
