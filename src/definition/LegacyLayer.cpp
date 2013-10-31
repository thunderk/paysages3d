#include "LegacyLayer.h"

LegacyLayer::LegacyLayer(BaseDefinition* parent, LayerType* type):
    BaseDefinition(parent), type(*type)
{
    legacy = type->callback_create();
    setName(getName());
}

LegacyLayer::~LegacyLayer()
{
    type.callback_delete(legacy);
}

void LegacyLayer::save(PackStream* pack)
{
    BaseDefinition::save(pack);
    type.callback_save(pack, legacy);
}

void LegacyLayer::load(PackStream* pack)
{
    BaseDefinition::load(pack);
    type.callback_load(pack, legacy);
}

void LegacyLayer::copy(BaseDefinition* destination)
{
    BaseDefinition::copy(destination);
    type.callback_copy(legacy, ((LegacyLayer*)destination)->legacy);
}

void LegacyLayer::validate()
{
    BaseDefinition::validate();
    type.callback_validate(legacy);
}

void LegacyLayer::setName(QString name)
{
    BaseDefinition::setName(name);
    qstrncpy(legacy_name, name.toUtf8().constData(), 99);
}
