#include "VegetationLayerDefinition.h"

#include "TerrainHeightMap.h"
#include "VegetationModelDefinition.h"

VegetationLayerDefinition::VegetationLayerDefinition(DefinitionNode* parent) :
    DefinitionNode(parent, "layer")
{
    //area = new TerrainHeightMap();
    model = new VegetationModelDefinition(this);
}

VegetationLayerDefinition::~VegetationLayerDefinition()
{
    //delete area;
    delete model;
}

void VegetationLayerDefinition::save(PackStream *stream) const
{
    //area->save(stream);
    model->save(stream);
}

void VegetationLayerDefinition::load(PackStream *stream)
{
    //area->load(stream);
    model->load(stream);
}

void VegetationLayerDefinition::copy(DefinitionNode *destination_) const
{
    VegetationLayerDefinition *destination = (VegetationLayerDefinition *)destination_;

    //area->copy(destination->area);
    model->copy(destination->model);
}

void VegetationLayerDefinition::validate()
{
    //area->validate();
    model->validate();
}

void VegetationLayerDefinition::applyPreset(VegetationLayerPreset preset)
{
    // TODO
    model->randomize();
}
