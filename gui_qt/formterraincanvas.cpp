#include "formterraincanvas.h"

/**************** Form ****************/
FormTerrainCanvas::FormTerrainCanvas(QWidget *parent, Layers* layers):
    BaseFormLayer(parent, layers)
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
