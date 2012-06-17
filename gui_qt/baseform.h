#ifndef _PAYSAGES_QT_BASEFORM_H_
#define _PAYSAGES_QT_BASEFORM_H_

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include "basepreview.h"
#include "baseinput.h"
#include "../lib_paysages/shared/types.h"
#include "../lib_paysages/noise.h"
#include "../lib_paysages/curve.h"
#include "../lib_paysages/color.h"

class BaseForm:public QWidget
{
    Q_OBJECT

public:
    BaseForm(QWidget* parent, bool auto_apply=false, bool with_layers=false);
    void hideButtons();

signals:
    void configApplied();

public slots:
    virtual void revertConfig();
    virtual void applyConfig();

protected slots:
    virtual void configChangeEvent();
    
private slots:
    void layerAddClicked();
    void layerDelClicked();
    void layerListChanged();

protected:
    void addPreview(BasePreview* preview, QString label);
    QPushButton* addButton(QString label);
    void addInput(BaseInput* input);
    void addInputInt(QString label, int* value, int min, int max, int small_step, int large_step);
    void addInputDouble(QString label, double* value, double min, double max, double small_step, double large_step);
    void addInputBoolean(QString label, int* value);
    void addInputColor(QString label, Color* value);
    void addInputColorGradation(QString label, ColorGradation* value);
    void addInputNoise(QString label, NoiseGenerator* value);
    void addInputCurve(QString label, Curve* value, double xmin, double xmax, double ymin, double ymax);
    void addInputMaterial(QString label, SurfaceMaterial* material);
    
    int currentLayer();
    void setLayerCount(int layer_count);
    
    virtual void layerAddedEvent();
    virtual void layerDeletedEvent(int layer);
    virtual void layerSelectedEvent(int layer);

private:
    bool auto_apply;
    bool with_layers;
    QComboBox* layer_list;
    QPushButton* layer_new;
    QPushButton* layer_del;
    QWidget* previews;
    QWidget* form;
    QWidget* form_labels;
    QWidget* form_previews;
    QWidget* form_controls;
    QWidget* buttons;
    QPushButton* button_apply;
    QPushButton* button_revert;
};

#endif // _PAYSAGES_QT_BASEFORM_H_
