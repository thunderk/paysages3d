#ifndef _PAYSAGES_QT_FORMSKY_H_
#define _PAYSAGES_QT_FORMSKY_H_

#include <QWidget>
#include "preview.h"
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
    Preview* previewEast;
    Preview* previewWest;
};

#endif // _PAYSAGES_QT_FORMSKY_H_
