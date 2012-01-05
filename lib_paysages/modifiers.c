#include "shared/types.h"
#include "shared/functions.h"
#include "modifiers.h"

#include <stdlib.h>

typedef enum
{
    MODE_NULL,
    MODE_ADD_VALUE,
    MODE_FIX_VALUE
} _EnumMode;

struct HeightModifier
{
    Zone* zone;
    _EnumMode mode;
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

void modifierSave(HeightModifier* modifier, FILE* f)
{
    toolsSaveInt(f, modifier->mode);
    toolsSaveDouble(f, modifier->value);
    zoneSave(modifier->zone, f);
}

void modifierLoad(HeightModifier* modifier, FILE* f)
{
    modifier->mode = toolsLoadInt(f);
    modifier->value = toolsLoadDouble(f);
    zoneLoad(modifier->zone, f);
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
