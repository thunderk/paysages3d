#ifndef TEXTURESDEFINITION_H
#define TEXTURESDEFINITION_H

#include "definition_global.h"

#include "Layers.h"

namespace paysages {
namespace definition {

class DEFINITIONSHARED_EXPORT TexturesDefinition : public Layers
{
public:
    TexturesDefinition(DefinitionNode *parent);

    inline TextureLayerDefinition* getTextureLayer(int position) const {return (TextureLayerDefinition*)getLayer(position);}

    typedef enum
    {
        TEXTURES_PRESET_FULL,
        TEXTURES_PRESET_IRELAND,
        TEXTURES_PRESET_ALPS,
        TEXTURES_PRESET_CANYON
    } TexturesPreset;
    void applyPreset(TexturesPreset preset);

    double getMaximalDisplacement();
};

}
}

#endif // TEXTURESDEFINITION_H
