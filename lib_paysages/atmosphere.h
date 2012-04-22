#ifndef _PAYSAGES_ATMOSPHERE_H_
#define _PAYSAGES_ATMOSPHERE_H_

#include "shared/types.h"
#include "pack.h"
#include "lighting.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    double distance_near;
    double distance_far;
    double full_mask;
    int auto_lock_on_haze;
    Color color;
} AtmosphereDefinition;

void atmosphereInit();
void atmosphereQuit();
void atmosphereSave(PackStream* stream, AtmosphereDefinition* definition);
void atmosphereLoad(PackStream* stream, AtmosphereDefinition* definition);

AtmosphereDefinition atmosphereCreateDefinition();
void atmosphereDeleteDefinition(AtmosphereDefinition* definition);
void atmosphereCopyDefinition(AtmosphereDefinition* source, AtmosphereDefinition* destination);
void atmosphereValidateDefinition(AtmosphereDefinition* definition);

Color atmosphereApply(AtmosphereDefinition* definition, Renderer* renderer, Vector3 location, Color base);

#ifdef __cplusplus
}
#endif

#endif
