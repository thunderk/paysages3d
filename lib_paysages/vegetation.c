#include "vegetation.h"

#define MAX_LAYER_COUNT 50

struct VegetationDefinition {
    int nb_layers;
    VegetationLayerDefinition layers[MAX_LAYER_COUNT];
};

void vegetationInit()
{
}

void vegetationQuit()
{
}

void vegetationSave(PackStream* stream, VegetationDefinition* definition)
{
}

void vegetationLoad(PackStream* stream, VegetationDefinition* definition)
{
}

VegetationDefinition* vegetationCreateDefinition()
{
}

void vegetationDeleteDefinition(VegetationDefinition* definition)
{
}

void vegetationCopyDefinition(VegetationDefinition* source, VegetationDefinition* destination)
{
}

void vegetationValidateDefinition(VegetationDefinition* definition)
{
}

VegetationLayerDefinition vegetationLayerCreateDefinition()
{
}

void vegetationLayerDeleteDefinition(VegetationLayerDefinition* definition)
{
}

void vegetationLayerCopyDefinition(VegetationLayerDefinition* source, VegetationLayerDefinition* destination)
{
}

void vegetationLayerValidateDefinition(VegetationLayerDefinition* definition)
{
}

int vegetationGetLayerCount(VegetationDefinition* definition)
{
}

int vegetationGetLayer(VegetationDefinition* definition, int position, VegetationLayerDefinition* layer)
{
}

int vegetationSetLayer(VegetationDefinition* definition, int position, VegetationLayerDefinition* layer)
{
}

int vegetationAddLayer(VegetationDefinition* definition)
{
}

void vegetationDeleteLayer(VegetationDefinition* definition, int layer)
{
}
