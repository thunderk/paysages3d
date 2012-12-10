#ifndef _PAYSAGES_QT_FORMTERRAIN_H_
#define _PAYSAGES_QT_FORMTERRAIN_H_

#include <QWidget>
#include "basepreview.h"
#include "baseform.h"

class FormTerrain : public BaseForm
{
    Q_OBJECT

public:
    explicit FormTerrain(QWidget *parent = 0);

public slots:
    virtual void revertConfig();
    virtual void applyConfig();

protected slots:
    virtual void configChangeEvent();

private slots:
    void startPainting();

private:
    BasePreview* previewHeight;
    BasePreview* previewColor;
};

#endif
