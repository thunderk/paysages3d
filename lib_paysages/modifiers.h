#ifndef _PAYSAGES_MODIFIERS_H_
#define _PAYSAGES_MODIFIERS_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HeightModifier HeightModifier;

HeightModifier* modifierCreate();
HeightModifier* modifierCreateCopy(HeightModifier* source);
void modifierDelete(HeightModifier* modifier);
void modifierSave(HeightModifier* modifier, FILE* f);
void modifierLoad(HeightModifier* modifier, FILE* f);
Zone* modifierGetZone(HeightModifier* modifier);
void modifierActionAddValue(HeightModifier* modifier, double value);
void modifierActionFixValue(HeightModifier* modifier, double value);
Vector3 modifierApply(HeightModifier* modifier, Vector3 location);

#ifdef __cplusplus
}
#endif

#endif
