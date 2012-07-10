#include "formterraincanvas.h"

/**************** Form ****************/
FormTerrainCanvas::FormTerrainCanvas(QWidget *parent):
    BaseFormLayer(parent)
{
}

void FormTerrainCanvas::revertConfig()
{
    BaseFormLayer::revertConfig();
}

void FormTerrainCanvas::applyConfig()
{
    BaseFormLayer::applyConfig();
}

void FormTerrainCanvas::configChangeEvent()
{
    BaseFormLayer::configChangeEvent();
}
