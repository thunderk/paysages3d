#ifndef _PAYSAGES_QT_FORMTERRAINCANVAS_H_
#define _PAYSAGES_QT_FORMTERRAINCANVAS_H_

#include <QWidget>
#include "basepreview.h"
#include "baseformlayer.h"

class FormTerrainCanvas : public BaseFormLayer
{
    Q_OBJECT

public:
    FormTerrainCanvas(QWidget* parent);

public slots:
    virtual void revertConfig();
    virtual void applyConfig();

protected slots:
    virtual void configChangeEvent();

private:
//    BasePreview* previewHeight;
//    BasePreview* previewColor;
};

#endif
