#ifndef _PAYSAGES_QT_FORMTEXTURES_H_
#define _PAYSAGES_QT_FORMTEXTURES_H_

#include <QWidget>
#include "preview.h"
#include "baseform.h"

class FormTextures : public BaseForm
{
    Q_OBJECT

public:
    explicit FormTextures(QWidget *parent = 0);

public slots:
    virtual void revertConfig();
    virtual void applyConfig();

protected slots:
    virtual void configChangeEvent();

private:
    Preview* previewCoverage;
    Preview* previewColor;
};

#endif
