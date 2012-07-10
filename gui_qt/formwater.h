#ifndef _PAYSAGES_QT_FORMWATER_H_
#define _PAYSAGES_QT_FORMWATER_H_

#include <QWidget>
#include "basepreview.h"
#include "baseform.h"

class FormWater : public BaseForm
{
    Q_OBJECT

public:
    explicit FormWater(QWidget *parent = 0);

public slots:
    virtual void revertConfig();
    virtual void applyConfig();

private:
    BasePreview* previewCoverage;
    BasePreview* previewColor;
};

#endif
