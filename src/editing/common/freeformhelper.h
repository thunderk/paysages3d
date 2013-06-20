#ifndef FREEFORMHELPER_H
#define FREEFORMHELPER_H

#include "widgetsliderdecimal.h"
#include "../basepreview.h"

class QSlider;
class QPushButton;

class FreeFormHelper:public QObject
{
    Q_OBJECT

public:
    FreeFormHelper(QWidget* form_widget);
    ~FreeFormHelper();

    void startManaging();

    void addPreview(BasePreview* preview, PreviewRenderer* renderer);
    void addPreview(QString widget_name, PreviewRenderer* renderer);

    void addDoubleInputSlider(WidgetSliderDecimal* slider, double* value, double min=0.0, double max=1.0, double small_step=0.0, double large_step=0.0);
    void addDoubleInputSlider(QString widget_name, double* value, double min=0.0, double max=1.0, double small_step=0.0, double large_step=0.0);

    void setApplyButton(QPushButton* button);
    void setApplyButton(QString widget_name);

    void setRevertButton(QPushButton* button);
    void setRevertButton(QString widget_name);

    void setExploreButton(QPushButton* button);
    void setExploreButton(QString widget_name);

    void setRenderButton(QPushButton* button);
    void setRenderButton(QString widget_name);

    void setLabelText(QLabel* label, QString text);
    void setLabelText(QString widget_name, QString text);

    void openDialog(QDialog* dialog);
    void gotoMainTab(int position);

signals:
    void revertClicked();
    void applyClicked();

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

protected:
    bool eventFilter(QObject* object, QEvent* event);

private:
    QWidget* _form_widget;

    bool _data_changed;

    QVector<BasePreview*> _previews;
    QVector<WidgetSliderDecimal*> _inputs_decimal;

    QPushButton* _button_apply;
    QPushButton* _button_revert;
    QPushButton* _button_explore;
    QPushButton* _button_render;
};

#endif
