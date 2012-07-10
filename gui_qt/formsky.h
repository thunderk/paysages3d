#ifndef _PAYSAGES_QT_FORMSKY_H_
#define _PAYSAGES_QT_FORMSKY_H_

#include <QWidget>
#include "basepreview.h"
#include "baseform.h"

class FormSky : public BaseForm
{
    Q_OBJECT

public:
    explicit FormSky(QWidget *parent = 0);

public slots:
    virtual void revertConfig();
    virtual void applyConfig();

protected slots:
    virtual void configChangeEvent();

private:
    BasePreview* previewEast;
    BasePreview* previewWest;
};

#endif
