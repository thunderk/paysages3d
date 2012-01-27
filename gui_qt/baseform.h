#ifndef _PAYSAGES_QT_BASEFORM_H_
#define _PAYSAGES_QT_BASEFORM_H_

#include <QWidget>
#include <QPushButton>
#include "preview.h"
#include "baseinput.h"
#include "../lib_paysages/shared/types.h"
#include "../lib_paysages/noise.h"

class BaseForm:public QWidget
{
    Q_OBJECT

public:
    BaseForm(QWidget* parent, bool auto_apply=false);

signals:
    void configApplied();

public slots:
    virtual void revertConfig();
    virtual void applyConfig();

protected slots:
    virtual void configChangeEvent();

protected:
    void addPreview(Preview* preview, QString label);
    QPushButton* addButton(QString label);
    void addInput(BaseInput* input);
    void addInputInt(QString label, int* value, int min, int max, int small_step, int large_step);
    void addInputDouble(QString label, double* value, double min, double max, double small_step, double large_step);
    void addInputColor(QString label, Color* value);
    void addInputColorGradation(QString label, ColorGradation* value);
    void addInputNoise(QString label, NoiseGenerator* value);

private:
    bool auto_apply;
    QWidget* previews;
    QWidget* form;
    QWidget* buttons;
    QPushButton* button_apply;
    QPushButton* button_revert;
};

#endif // _PAYSAGES_QT_BASEFORM_H_
