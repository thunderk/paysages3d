#ifndef _PAYSAGES_QT_BASEFORMLAYER_H_
#define _PAYSAGES_QT_BASEFORMLAYER_H_

/* Base form, with automatic layer control */

#include "baseform.h"
#include "rendering/layers.h"

class BaseFormLayer:public BaseForm
{
    Q_OBJECT

public:
    BaseFormLayer(QWidget* parent, Layers* layers=NULL);
    ~BaseFormLayer();

public slots:
    virtual void revertConfig();
    virtual void applyConfig();

protected:
    void setLayers(Layers* layers);
    virtual void layerReadCurrentFrom(void* layer_definition) = 0;
    virtual void layerWriteCurrentTo(void* layer_definition) = 0;
    virtual void afterLayerAdded(void* layer_definition);

private:
    virtual QStringList getLayers();
    virtual void layerAddedEvent();
    virtual void layerDeletedEvent(int layer);
    virtual void layerMovedEvent(int layer, int new_position);
    virtual void layerRenamedEvent(int layer, QString new_name);
    virtual void layerSelectedEvent(int layer);

protected slots:
    virtual void configChangeEvent();

private:
    Layers* _layers_original;
    Layers* _layers_modified;
};

#endif
