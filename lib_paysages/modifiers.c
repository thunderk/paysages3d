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
    double value;
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
    packWriteDouble(stream, &modifier->value);
    zoneSave(stream, modifier->zone);
}

void modifierLoad(PackStream* stream, HeightModifier* modifier)
{
    packReadInt(stream, &modifier->mode);
    packReadDouble(stream, &modifier->value);
    zoneLoad(stream, modifier->zone);
}

Zone* modifierGetZone(HeightModifier* modifier)
{
    return modifier->zone;
}

void modifierActionAddValue(HeightModifier* modifier, double value)
{
    modifier->mode = MODE_ADD_VALUE;
    modifier->value = value;
}

void modifierActionFixValue(HeightModifier* modifier, double value)
{
    modifier->mode = MODE_FIX_VALUE;
    modifier->value = value;
}

Vector3 modifierApply(HeightModifier* modifier, Vector3 location)
{
    double influence, diff;
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
