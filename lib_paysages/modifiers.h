#ifndef _PAYSAGES_MODIFIERS_H_
#define _PAYSAGES_MODIFIERS_H_

#include "pack.h"
#include "zone.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HeightModifier HeightModifier;

HeightModifier* modifierCreate();
HeightModifier* modifierCreateCopy(HeightModifier* source);
void modifierDelete(HeightModifier* modifier);
void modifierSave(PackStream* stream, HeightModifier* modifier);
void modifierLoad(PackStream* stream, HeightModifier* modifier);
Zone* modifierGetZone(HeightModifier* modifier);
void modifierActionAddValue(HeightModifier* modifier, float value);
void modifierActionFixValue(HeightModifier* modifier, float value);
Vector3 modifierApply(HeightModifier* modifier, Vector3 location);

#ifdef __cplusplus
}
#endif

#endif
