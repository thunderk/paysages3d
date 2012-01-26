#ifndef _PAYSAGES_QT_FORMLIGHTING_H_
#define _PAYSAGES_QT_FORMLIGHTING_H_

#include <QWidget>
#include "preview.h"
#include "baseform.h"

class FormLighting : public BaseForm
{
    Q_OBJECT

public:
    explicit FormLighting(QWidget *parent = 0);

public slots:
    virtual void revertConfig();
    virtual void applyConfig();

protected slots:
    virtual void configChangeEvent();

private:
    Preview* previewColor;
};

#endif
