#include "modifiers.h"

#include <stdlib.h>

#include "shared/types.h"
#include "tools.h"

#define MODE_NULL 0
#define MODE_ADD_VALUE 1
#define MODE_FIX_VALUE 2

struct HeightModifier
{
    Zone* zone;
    int mode;
    float value;
};

HeightModifier* modifierCreate()
{
    HeightModifier* modifier;
    
    modifier = (HeightModifier*)malloc(sizeof(HeightModifier));
    modifier->zone = zoneCreate();
    modifier->mode = MODE_NULL;
    
    return modifier;
}

HeightModifier* modifierCreateCopy(HeightModifier* source)
{
    HeightModifier* result;
    
    result = (HeightModifier*)malloc(sizeof(HeightModifier));
    result->zone = zoneCreate();
    zoneCopy(source->zone, result->zone);
    result->mode = source->mode;
    result->value = source->value;
    
    return result;
}

void modifierDelete(HeightModifier* modifier)
{
    zoneDelete(modifier->zone);
    free(modifier);
}

void modifierSave(PackStream* stream, HeightModifier* modifier)
{
    packWriteInt(stream, &modifier->mode);
    packWriteFloat(stream, &modifier->value);
    zoneSave(stream, modifier->zone);
}

void modifierLoad(PackStream* stream, HeightModifier* modifier)
{
    packReadInt(stream, &modifier->mode);
    packReadFloat(stream, &modifier->value);
    zoneLoad(stream, modifier->zone);
}

Zone* modifierGetZone(HeightModifier* modifier)
{
    return modifier->zone;
}

void modifierActionAddValue(HeightModifier* modifier, float value)
{
    modifier->mode = MODE_ADD_VALUE;
    modifier->value = value;
}

void modifierActionFixValue(HeightModifier* modifier, float value)
{
    modifier->mode = MODE_FIX_VALUE;
    modifier->value = value;
}

Vector3 modifierApply(HeightModifier* modifier, Vector3 location)
{
    float influence, diff;
    Vector3 normal;
    
    switch (modifier->mode)
    {
        case MODE_ADD_VALUE:
            influence = zoneGetValue(modifier->zone, location, normal);
            location.y += modifier->value * influence;
            break;
        case MODE_FIX_VALUE:
            influence = zoneGetValue(modifier->zone, location, normal);
            diff = modifier->value - location.y;
            location.y += diff * influence;
            break;
        case MODE_NULL:
            break;
    }
    
    return location;
}
