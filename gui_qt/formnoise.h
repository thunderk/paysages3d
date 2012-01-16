#ifndef _PAYSAGES_QT_FORMNOISE_H_
#define _PAYSAGES_QT_FORMNOISE_H_

#include <QWidget>
#include "preview.h"
#include "baseform.h"

#include "../lib_paysages/shared/types.h"

class FormNoise : public BaseForm
{
    Q_OBJECT

public:
    FormNoise(QWidget* parent, NoiseGenerator* noise);

public slots:
    virtual void revertConfig();
    virtual void applyConfig();

protected slots:
    virtual void applyConfigPreview();

private:
    Preview* previewLevel;
    Preview* previewTotal;
    NoiseGenerator* generator;
};

#endif
