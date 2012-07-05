#ifndef _PAYSAGES_QT_FORMCLOUDS_H_
#define _PAYSAGES_QT_FORMCLOUDS_H_

#include <QWidget>
#include "basepreview.h"
#include "baseform.h"

class FormClouds : public BaseForm
{
    Q_OBJECT

public:
    explicit FormClouds(QWidget *parent = 0);

public slots:
    virtual void revertConfig();
    virtual void applyConfig();

protected slots:
    virtual void configChangeEvent();
    
protected:
    virtual void layerAddedEvent();
    virtual void layerDeletedEvent(int layer);
    virtual void layerMovedEvent(int layer, int new_position);
    virtual void layerSelectedEvent(int layer);

private:
    BasePreview* previewCoverage;
    BasePreview* previewColor;
};

#endif
