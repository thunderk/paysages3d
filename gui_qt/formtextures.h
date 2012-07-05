#ifndef _PAYSAGES_QT_FORMTEXTURES_H_
#define _PAYSAGES_QT_FORMTEXTURES_H_

#include <QWidget>
#include "basepreview.h"
#include "baseform.h"

class FormTextures : public BaseForm
{
    Q_OBJECT

public:
    explicit FormTextures(QWidget *parent = 0);
    ~FormTextures();

protected:
    virtual QStringList getLayers();
    virtual void layerAddedEvent();
    virtual void layerDeletedEvent(int layer);
    virtual void layerMovedEvent(int layer, int new_position);
    virtual void layerSelectedEvent(int layer);

public slots:
    virtual void revertConfig();
    virtual void applyConfig();

protected slots:
    virtual void configChangeEvent();

private:
    BasePreview* previewCoverage;
    BasePreview* previewColor;
};

#endif
