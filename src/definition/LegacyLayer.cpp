#include "LegacyLayer.h"

LegacyLayer::LegacyLayer(BaseDefinition* parent, LayerType* type):
    BaseDefinition(parent), type(*type)
{
    legacy = type->callback_create();
}

LegacyLayer::~LegacyLayer()
{
    type.callback_delete(legacy);
}

void LegacyLayer::save(PackStream* pack)
{
    type.callback_save(pack, legacy);
}

void LegacyLayer::load(PackStream* pack)
{
    type.callback_load(pack, legacy);
}

void LegacyLayer::copy(LegacyLayer* destination)
{
    type.callback_copy(legacy, destination->legacy);
}

void LegacyLayer::validate()
{
    type.callback_validate(legacy);
}
