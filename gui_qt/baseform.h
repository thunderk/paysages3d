#ifndef _GUI_QT_BASEFORM_H_
#define _GUI_QT_BASEFORM_H_

#include <QWidget>
#include "preview.h"

class BaseForm:public QWidget
{
public:
    BaseForm(QWidget* parent);

public slots:
    virtual void revertConfig();

protected:
    void addPreview(Preview* preview, QString label);
    void addDoubleSlider(QString label, double* value, double min, double max, double small_step, double large_step);

private:
    QWidget* previews;
    QWidget* form;
};

#endif // _GUI_QT_BASEFORM_H_
