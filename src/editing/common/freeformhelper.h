#ifndef FREEFORMHELPER_H
#define FREEFORMHELPER_H

#include "editing_global.h"

#include <QVector>
#include <QStringList>

#include "widgetsliderdecimal.h"

class QSlider;
class QPushButton;
class QLabel;
class Renderer;

class FreeFormHelper:public QObject
{
    Q_OBJECT

public:
    FreeFormHelper(QWidget* form_widget);
    ~FreeFormHelper();

    void startManaging();

    void addPreview(BasePreview* preview, Base2dPreviewRenderer* renderer);
    void addDoubleInputSlider(WidgetSliderDecimal* slider, double* value, double min=0.0, double max=1.0, double small_step=0.0, double large_step=0.0);
    void setApplyButton(QPushButton* button);
    void setRevertButton(QPushButton* button);
    void setExploreButton(QPushButton* button);
    void setRenderButton(QPushButton* button);
    void setLabelText(QLabel* label, QString text);

    void setPresetButton(QPushButton* button);
    void addPreset(const QString& name);

    void openDialog(QDialog* dialog);
    void gotoMainTab(int position);

signals:
    void revertClicked();
    void applyClicked();

    void presetSelected(int position);

    void needLocalRefreshing();
    void needGlobalRefreshing();
    void needReverting();
    void needCommitting();
    void needAlterRenderer(Renderer* renderer);

public slots:
    void processDataChange();
    void processGlobalRefresh();
    void processRevertClicked();
    void processApplyClicked();
    void processExploreClicked();
    void processRenderClicked();
    void processDecimalChange(double value);
    void processPresetClicked();

protected:
    bool eventFilter(QObject* object, QEvent* event);

private:
    QWidget* _form_widget;

    bool _data_changed;

    QPushButton* _button_presets;
    QStringList _presets;

    QVector<BasePreview*> _previews;
    QVector<WidgetSliderDecimal*> _inputs_decimal;

    QPushButton* _button_apply;
    QPushButton* _button_revert;
    QPushButton* _button_explore;
    QPushButton* _button_render;
};

#endif
