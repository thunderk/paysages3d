#ifndef _PAYSAGES_VEGETATION_H_
#define _PAYSAGES_VEGETATION_H_

#include "pack.h"
#include "zone.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VEGETATION_MAX_LAYERS 50

typedef struct
{
    Zone* zone;
    float max_height;
} VegetationLayerDefinition;

typedef struct VegetationDefinition VegetationDefinition;

void vegetationInit();
void vegetationQuit();
void vegetationSave(PackStream* stream, VegetationDefinition* definition);
void vegetationLoad(PackStream* stream, VegetationDefinition* definition);

VegetationDefinition* vegetationCreateDefinition();
void vegetationDeleteDefinition(VegetationDefinition* definition);
void vegetationCopyDefinition(VegetationDefinition* source, VegetationDefinition* destination);
void vegetationValidateDefinition(VegetationDefinition* definition);

VegetationLayerDefinition vegetationLayerCreateDefinition();
void vegetationLayerDeleteDefinition(VegetationLayerDefinition* definition);
void vegetationLayerCopyDefinition(VegetationLayerDefinition* source, VegetationLayerDefinition* destination);
void vegetationLayerValidateDefinition(VegetationLayerDefinition* definition);

int vegetationGetLayerCount(VegetationDefinition* definition);
int vegetationGetLayer(VegetationDefinition* definition, int position, VegetationLayerDefinition* layer);
int vegetationSetLayer(VegetationDefinition* definition, int position, VegetationLayerDefinition* layer);
int vegetationAddLayer(VegetationDefinition* definition);
void vegetationDeleteLayer(VegetationDefinition* definition, int layer);

/*float vegetationGetLayerCoverage(TextureLayerDefinition* definition, Renderer* renderer, Vector3 location, float detail);*/

#ifdef __cplusplus
}
#endif

#endif
