#ifndef _PAYSAGES_QT_DIALOGLAYERS_H_
#define _PAYSAGES_QT_DIALOGLAYERS_H_

/* Dialog for generic layers control */

#include <QListWidget>
#include "tools.h"

#include "../lib_paysages/layers.h"

class DialogLayers;
class BaseFormLayer;

typedef BaseFormLayer* (*FormLayerBuilder)(DialogLayers* parent, Layers* layers);

class DialogLayers : public DialogWithPreview
{
    Q_OBJECT
public:
    explicit DialogLayers(QWidget* parent, Layers* layers, QString title, FormLayerBuilder form_builder);
    ~DialogLayers();

    static bool editLayers(QWidget* parent, Layers* layers, QString title, FormLayerBuilder form_builder);

public slots:
    virtual void accept();
    void revert();

protected:
    virtual void closeEvent(QCloseEvent* e);

private:
    BaseFormLayer* _form;
    Layers* _layers;
};

#endif
