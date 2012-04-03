#include "formtextures.h"

#include "../lib_paysages/textures.h"
#include "../lib_paysages/scenery.h"

static TexturesDefinition _definition;
static TextureLayerDefinition _layer;

/**************** Previews ****************/
class PreviewTexturesCoverage:public BasePreview
{
public:
    PreviewTexturesCoverage(QWidget* parent):BasePreview(parent)
    {
        _renderer = rendererCreate();
        _renderer.render_quality = 3;
        
        _preview_layer = texturesLayerCreateDefinition();
    }
protected:
    QColor getColor(double x, double y)
    {
        return QColor(0, 0, 0);
    }
    void updateData()
    {
        texturesLayerCopyDefinition(&_layer, &_preview_layer);
    }

private:
    Renderer _renderer;
    TextureLayerDefinition _preview_layer;
};

class PreviewTexturesColor:public BasePreview
{
public:
    PreviewTexturesColor(QWidget* parent):BasePreview(parent)
    {
        _renderer = rendererCreate();
        _renderer.render_quality = 3;
        
        _preview_layer = texturesLayerCreateDefinition();
    }
protected:
    QColor getColor(double x, double y)
    {
        return QColor(0, 0, 0);
    }
    void updateData()
    {
        texturesLayerCopyDefinition(&_layer, &_preview_layer);
    }
private:
    Renderer _renderer;
    TextureLayerDefinition _preview_layer;
};

/**************** Form ****************/
FormTextures::FormTextures(QWidget *parent):
    BaseForm(parent, false, true)
{
    _definition = texturesCreateDefinition();
    _layer = texturesLayerCreateDefinition();

    previewCoverage = new PreviewTexturesCoverage(this);
    previewColor = new PreviewTexturesColor(this);
    addPreview(previewCoverage, QString("Coverage preview"));
    addPreview(previewColor, QString("Colored preview"));

    addInputNoise(tr("Surface noise"), _layer.bump_noise);
    addInputDouble(tr("Surface noise height"), &_layer.bump_height, 0.0, 0.5, 0.001, 0.05);
    addInputDouble(tr("Surface noise scaling"), &_layer.bump_scaling, 0.1, 3.0, 0.01, 0.1);
    addInputColor(tr("Base color"), &_layer.material.base);
    addInputDouble(tr("Light reflection"), &_layer.material.reflection, 0.0, 1.0, 0.01, 0.1);
    addInputDouble(tr("Light reflection shininess"), &_layer.material.shininess, 0.0, 20.0, 0.1, 1.0);

    revertConfig();
}

void FormTextures::revertConfig()
{
    sceneryGetTextures(&_definition);
    setLayerCount(texturesGetLayerCount(&_definition));
    BaseForm::revertConfig();
}

void FormTextures::applyConfig()
{
    scenerySetTextures(&_definition);
    BaseForm::applyConfig();
}

void FormTextures::configChangeEvent()
{
    texturesLayerCopyDefinition(&_layer, texturesGetLayer(&_definition, currentLayer()));
    texturesValidateDefinition(&_definition);
    BaseForm::configChangeEvent();
}

void FormTextures::layerAddedEvent()
{
    if (texturesAddLayer(&_definition) >= 0)
    {
        BaseForm::layerAddedEvent();
    }
}

void FormTextures::layerDeletedEvent(int layer)
{
    texturesDeleteLayer(&_definition, layer);
    
    BaseForm::layerDeletedEvent(layer);
}

void FormTextures::layerSelectedEvent(int layer)
{
    texturesLayerCopyDefinition(texturesGetLayer(&_definition, layer), &_layer);
    
    BaseForm::layerSelectedEvent(layer);
}
