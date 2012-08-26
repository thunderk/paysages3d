#include "noise.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "tools.h"
#include "simplexnoise.h"

#define MAX_LEVEL_COUNT 30

struct NoiseLevel;

struct NoiseGenerator
{
    double height_offset;
    int level_count;
    struct NoiseLevel levels[MAX_LEVEL_COUNT];
    
    double _max_height;
};

void noiseInit()
{
    simplexNoiseInit();
}

void noiseQuit()
{
}

void noiseSave(PackStream* stream)
{
    UNUSED(stream);
}

void noiseLoad(PackStream* stream)
{
    UNUSED(stream);
}

NoiseGenerator* noiseCreateGenerator()
{
    NoiseGenerator* result;

    /* initialize */
    result = malloc(sizeof(NoiseGenerator));
    result->level_count = 0;
    result->height_offset = 0.0;
    
    noiseValidate(result);

    return result;
}

void noiseDeleteGenerator(NoiseGenerator* generator)
{
    free(generator);
}

void noiseSaveGenerator(PackStream* stream, NoiseGenerator* generator)
{
    int x;

    packWriteDouble(stream, &generator->height_offset);
    packWriteInt(stream, &generator->level_count);

    for (x = 0; x < generator->level_count; x++)
    {
        NoiseLevel* level = generator->levels + x;
        
        packWriteDouble(stream, &level->scaling);
        packWriteDouble(stream, &level->height);
        packWriteDouble(stream, &level->xoffset);
        packWriteDouble(stream, &level->yoffset);
        packWriteDouble(stream, &level->zoffset);
    }
}

void noiseLoadGenerator(PackStream* stream, NoiseGenerator* generator)
{
    int x;

    packReadDouble(stream, &generator->height_offset);
    packReadInt(stream, &generator->level_count);

    for (x = 0; x < generator->level_count; x++)
    {
        NoiseLevel* level = generator->levels + x;
        
        packReadDouble(stream, &level->scaling);
        packReadDouble(stream, &level->height);
        packReadDouble(stream, &level->xoffset);
        packReadDouble(stream, &level->yoffset);
        packReadDouble(stream, &level->zoffset);
    }
    
    noiseValidate(generator);
}

void noiseCopy(NoiseGenerator* source, NoiseGenerator* destination)
{
    destination->height_offset = source->height_offset;
    destination->level_count = source->level_count;

    memcpy(destination->levels, source->levels, sizeof(NoiseLevel) * destination->level_count);
    
    noiseValidate(destination);
}

void noiseValidate(NoiseGenerator* generator)
{
    int x;
    double max_height = generator->height_offset;

    for (x = 0; x < generator->level_count; x++)
    {
        max_height += generator->levels[x].height / 2.0;
    }

    generator->_max_height = max_height;
}

void noiseForceValue(NoiseGenerator* generator, double value)
{
    noiseClearLevels(generator);
    generator->height_offset = value;
    noiseAddLevelSimple(generator, 1.0, 0.0); /* FIXME Should not be needed */
}

double noiseGetMaxValue(NoiseGenerator* generator)
{
    return generator->_max_height;
}

int noiseGetLevelCount(NoiseGenerator* generator)
{
    return generator->level_count;
}

void noiseClearLevels(NoiseGenerator* generator)
{
    generator->level_count = 0;
    noiseValidate(generator);
}

void noiseAddLevel(NoiseGenerator* generator, NoiseLevel level)
{
    if (generator->level_count < MAX_LEVEL_COUNT)
    {
        generator->levels[generator->level_count] = level;
        generator->level_count++;
        noiseValidate(generator);
    }
}

void noiseAddLevelSimple(NoiseGenerator* generator, double scaling, double height)
{
    NoiseLevel level;

    level.scaling = scaling;
    level.height = height;
    level.xoffset = toolsRandom();
    level.yoffset = toolsRandom();
    level.zoffset = toolsRandom();

    noiseAddLevel(generator, level);
}

void noiseAddLevels(NoiseGenerator* generator, int level_count, NoiseLevel start_level, double scaling_factor, double height_factor, int randomize_offset)
{
    int i;

    for (i = 0; i < level_count; i++)
    {
        if (randomize_offset)
        {
            start_level.xoffset = toolsRandom();
            start_level.yoffset = toolsRandom();
            start_level.zoffset = toolsRandom();
        }
        noiseAddLevel(generator, start_level);
        start_level.scaling *= scaling_factor;
        start_level.height *= height_factor;
    }
}

void noiseAddLevelsSimple(NoiseGenerator* generator, int level_count, double scaling, double height)
{
    NoiseLevel level;

    level.scaling = scaling;
    level.height = height;
    noiseAddLevels(generator, level_count, level, 0.5, 0.5, 1);
}

void noiseRemoveLevel(NoiseGenerator* generator, int level)
{
    if (level >= 0 && level < generator->level_count)
    {
        if (generator->level_count > 1 && level < generator->level_count - 1)
        {
            memmove(generator->levels + level, generator->levels + level + 1, sizeof(NoiseLevel) * (generator->level_count - level - 1));
        }
        generator->level_count--;
        noiseValidate(generator);
    }
}

int noiseGetLevel(NoiseGenerator* generator, int level, NoiseLevel* params)
{
    if (level >= 0 && level < generator->level_count)
    {
        *params = generator->levels[level];
        return 1;
    }
    else
    {
        return 0;
    }
}

void noiseSetLevel(NoiseGenerator* generator, int level, NoiseLevel params)
{
    if (level >= 0 && level < generator->level_count)
    {
        generator->levels[level] = params;
        noiseValidate(generator);
    }
}

void noiseSetLevelSimple(NoiseGenerator* generator, int level, double scaling, double height)
{
    NoiseLevel params;

    params.scaling = scaling;
    params.height = height;
    params.xoffset = toolsRandom();
    params.yoffset = toolsRandom();
    params.zoffset = toolsRandom();

    noiseSetLevel(generator, level, params);
}

void noiseNormalizeHeight(NoiseGenerator* generator, double min_height, double max_height, int adjust_scaling)
{
    int level;
    double height = 0.0;
    double target_height = max_height - min_height;

    if (generator->level_count == 0)
    {
        return;
    }

    for (level = 0; level < generator->level_count; level++)
    {
        height += generator->levels[level].height;
    }
    for (level = 0; level < generator->level_count; level++)
    {
        generator->levels[level].height *= target_height / height;
        if (adjust_scaling)
        {
            generator->levels[level].scaling *= target_height / height;
        }
    }
    generator->height_offset = min_height + target_height / 2.0;
    noiseValidate(generator);
}




static inline double _get1DRawNoiseValue(NoiseGenerator* generator, double x)
{
    return simplexNoiseGet2DValue(x, 0.0) * 0.5;
}

static inline double _get1DLevelValue(NoiseGenerator* generator, NoiseLevel* level, double x)
{
    return _get1DRawNoiseValue(generator, x / level->scaling + level->xoffset) * level->height;
}

double noiseGet1DLevel(NoiseGenerator* generator, int level, double x)
{
    if (level >= 0 && level < generator->level_count)
    {
        return _get1DLevelValue(generator, generator->levels + level, x);
    }
    else
    {
        return 0.0;
    }
}

double noiseGet1DTotal(NoiseGenerator* generator, double x)
{
    int level;
    double result;

    result = 0.0;
    for (level = 0; level < generator->level_count; level++)
    {
        result += _get1DLevelValue(generator, generator->levels + level, x);
    }
    return result + generator->height_offset;
}

double noiseGet1DDetail(NoiseGenerator* generator, double x, double detail)
{
    int level;
    double result, height, factor;

    result = 0.0;
    for (level = 0; level < generator->level_count; level++)
    {
        height = generator->levels[level].height;
        factor = 1.0;
        if (height < detail * 0.25)
        {
            break;
        }
        else if (height < detail * 0.5)
        {
            factor = (detail * 0.5 - height) / 0.25;
        }

        result += _get1DLevelValue(generator, generator->levels + level, x) * factor;
    }
    return result + generator->height_offset;
}




static inline double _get2DRawNoiseValue(NoiseGenerator* generator, double x, double y)
{
    return simplexNoiseGet2DValue(x, y) * 0.5;
}

static inline double _get2DLevelValue(NoiseGenerator* generator, NoiseLevel* level, double x, double y)
{
    return _get2DRawNoiseValue(generator, x / level->scaling + level->xoffset, y / level->scaling + level->yoffset) * level->height;
}

double noiseGet2DLevel(NoiseGenerator* generator, int level, double x, double y)
{
    if (level >= 0 && level < generator->level_count)
    {
        return _get2DLevelValue(generator, generator->levels + level, x, y);
    }
    else
    {
        return 0.0;
    }
}

double noiseGet2DTotal(NoiseGenerator* generator, double x, double y)
{
    int level;
    double result;

    result = 0.0;
    for (level = 0; level < generator->level_count; level++)
    {
        result += _get2DLevelValue(generator, generator->levels + level, x, y);
    }
    return result + generator->height_offset;
}

double noiseGet2DDetail(NoiseGenerator* generator, double x, double y, double detail)
{
    int level;
    double result, height, factor;

    result = 0.0;
    for (level = 0; level < generator->level_count; level++)
    {
        height = generator->levels[level].height;
        factor = 1.0;
        if (height < detail * 0.25)
        {
            break;
        }
        else if (height < detail * 0.5)
        {
            factor = (detail * 0.5 - height) / 0.25;
        }

        result += _get2DLevelValue(generator, generator->levels + level, x, y) * factor;
    }
    return result + generator->height_offset;
}




static inline double _get3DRawNoiseValue(NoiseGenerator* generator, double x, double y, double z)
{
    return simplexNoiseGet3DValue(x, y, z) * 0.5;
}

static inline double _get3DLevelValue(NoiseGenerator* generator, NoiseLevel* level, double x, double y, double z)
{
    return _get3DRawNoiseValue(generator, x / level->scaling + level->xoffset, y / level->scaling + level->yoffset, z / level->scaling + level->zoffset) * level->height;
}

double noiseGet3DLevel(NoiseGenerator* generator, int level, double x, double y, double z)
{
    if (level >= 0 && level < generator->level_count)
    {
        return _get3DLevelValue(generator, generator->levels + level, x, y, z);
    }
    else
    {
        return 0.0;
    }
}

double noiseGet3DTotal(NoiseGenerator* generator, double x, double y, double z)
{
    int level;
    double result;

    result = 0.0;
    for (level = 0; level < generator->level_count; level++)
    {
        result += _get3DLevelValue(generator, generator->levels + level, x, y, z);
    }
    return result + generator->height_offset;
}

double noiseGet3DDetail(NoiseGenerator* generator, double x, double y, double z, double detail)
{
    int level;
    double result, height, factor;

    result = 0.0;
    for (level = 0; level < generator->level_count; level++)
    {
        height = generator->levels[level].height;
        factor = 1.0;
        if (height < detail * 0.25)
        {
            break;
        }
        else if (height < detail * 0.5)
        {
            factor = (detail * 0.5 - height) / 0.25;
        }

        result += _get3DLevelValue(generator, generator->levels + level, x, y, z) * factor;
    }
    return result + generator->height_offset;
}
