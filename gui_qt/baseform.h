#ifndef _GUI_QT_BASEFORM_H_
#define _GUI_QT_BASEFORM_H_

#include <QWidget>
#include "preview.h"
#include "baseinput.h"
#include "../lib_paysages/shared/types.h"

class BaseForm:public QWidget
{
    Q_OBJECT

public:
    BaseForm(QWidget* parent);

public slots:
    virtual void revertConfig();
    virtual void applyConfigPreview();

protected:
    void addPreview(Preview* preview, QString label);
    void addInput(BaseInput* input);
    void addInputDouble(QString label, double* value, double min, double max, double small_step, double large_step);
    void addInputColor(QString label, Color* value);

private:
    QWidget* previews;
    QWidget* form;
};

#endif // _GUI_QT_BASEFORM_H_
