#include "noise.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "tools.h"
#include "noisesimplex.h"
#include "noisenaive.h"
#include "noiseperlin.h"
#include "opencl.h"

#define MAX_LEVEL_COUNT 30

struct NoiseGenerator
{
    NoiseFunction function;
    double height_offset;
    int level_count;
    NoiseLevel levels[MAX_LEVEL_COUNT];

    double _min_value;
    double _max_value;
    double (*_func_noise_1d)(double x);
    double (*_func_noise_2d)(double x, double y);
    double (*_func_noise_3d)(double x, double y, double z);
};

void noiseInit()
{
    noiseSimplexInit();
    noisePerlinInit();
    noiseNaiveInit();

    /* Noise stats */
    /*NoiseGenerator* noise;
    int x;
    double min, max, value;
    noise = noiseCreateGenerator();
    noiseSetFunctionParams(noise, NOISE_FUNCTION_NAIVE, 0.0);
    noiseAddLevelSimple(noise, 1.0, 0.0, 1.0);
    min = 100000.0;
    max = -100000.0;
    for (x = 0; x < 1000000; x++)
    {
        value = noiseGet1DTotal(noise, toolsRandom() * 10000.0);
        value = noiseGet2DTotal(noise, toolsRandom() * 10000.0, toolsRandom() * 10000.0);
        value = noiseGet3DTotal(noise, toolsRandom() * 10000.0, toolsRandom() * 10000.0, toolsRandom() * 10000.0);
        if (value < min) min = value;
        if (value > max) max = value;
    }
    printf("%f %f\n", min, max);
    noiseDeleteGenerator(noise);*/
}

void noiseQuit()
{
    noiseNaiveQuit();
}

void noiseSave(PackStream* stream)
{
    noiseNaiveSave(stream);
}

void noiseLoad(PackStream* stream)
{
    noiseNaiveLoad(stream);
}

NoiseGenerator* noiseCreateGenerator()
{
    NoiseGenerator* result;

    /* initialize */
    result = new NoiseGenerator;
    result->function.algorithm = NOISE_FUNCTION_SIMPLEX;
    result->function.ridge_factor = 0.0;
    result->function.curve_factor = 0.0;
    result->level_count = 0;
    result->height_offset = 0.0;

    noiseRandomizeOffsets(result);
    noiseValidate(result);

    return result;
}

void noiseDeleteGenerator(NoiseGenerator* generator)
{
    delete generator;
}

void noiseSaveGenerator(PackStream* stream, NoiseGenerator* generator)
{
    int x;

    x = (int)generator->function.algorithm;
    packWriteInt(stream, &x);
    packWriteDouble(stream, &generator->function.ridge_factor);
    packWriteDouble(stream, &generator->function.curve_factor);

    packWriteDouble(stream, &generator->height_offset);
    packWriteInt(stream, &generator->level_count);

    for (x = 0; x < generator->level_count; x++)
    {
        NoiseLevel* level = generator->levels + x;

        packWriteDouble(stream, &level->wavelength);
        packWriteDouble(stream, &level->amplitude);
        packWriteDouble(stream, &level->minvalue);
        packWriteDouble(stream, &level->xoffset);
        packWriteDouble(stream, &level->yoffset);
        packWriteDouble(stream, &level->zoffset);
    }
}

void noiseLoadGenerator(PackStream* stream, NoiseGenerator* generator)
{
    int x;

    packReadInt(stream, &x);
    generator->function.algorithm = (NoiseFunctionAlgorithm)x;
    packReadDouble(stream, &generator->function.ridge_factor);
    packReadDouble(stream, &generator->function.curve_factor);

    packReadDouble(stream, &generator->height_offset);
    packReadInt(stream, &generator->level_count);

    for (x = 0; x < generator->level_count; x++)
    {
        NoiseLevel* level = generator->levels + x;

        packReadDouble(stream, &level->wavelength);
        packReadDouble(stream, &level->amplitude);
        packReadDouble(stream, &level->minvalue);
        packReadDouble(stream, &level->xoffset);
        packReadDouble(stream, &level->yoffset);
        packReadDouble(stream, &level->zoffset);
    }

    noiseValidate(generator);
}

void noiseCopy(NoiseGenerator* source, NoiseGenerator* destination)
{
    destination->function = source->function;
    destination->height_offset = source->height_offset;
    destination->level_count = source->level_count;

    memcpy(destination->levels, source->levels, sizeof(NoiseLevel) * destination->level_count);

    noiseValidate(destination);
}

void noiseValidate(NoiseGenerator* generator)
{
    int x;

    if (generator->function.algorithm < 0 || generator->function.algorithm > NOISE_FUNCTION_NAIVE)
    {
        generator->function.algorithm = NOISE_FUNCTION_SIMPLEX;
    }
    switch (generator->function.algorithm)
    {
    case NOISE_FUNCTION_SIMPLEX:
        generator->_func_noise_1d = noiseSimplexGet1DValue;
        generator->_func_noise_2d = noiseSimplexGet2DValue;
        generator->_func_noise_3d = noiseSimplexGet3DValue;
        break;
    case NOISE_FUNCTION_PERLIN:
        generator->_func_noise_1d = noisePerlinGet1DValue;
        generator->_func_noise_2d = noisePerlinGet2DValue;
        generator->_func_noise_3d = noisePerlinGet3DValue;
        break;
    case NOISE_FUNCTION_NAIVE:
        generator->_func_noise_1d = noiseNaiveGet1DValue;
        generator->_func_noise_2d = noiseNaiveGet2DValue;
        generator->_func_noise_3d = noiseNaiveGet3DValue;
        break;
    }

    if (generator->function.ridge_factor > 0.5)
    {
        generator->function.ridge_factor = 0.5;
    }
    if (generator->function.ridge_factor < -0.5)
    {
        generator->function.ridge_factor = -0.5;
    }
    if (generator->function.curve_factor > 1.0)
    {
        generator->function.curve_factor = 1.0;
    }
    if (generator->function.curve_factor < -1.0)
    {
        generator->function.curve_factor = -1.0;
    }

    generator->_min_value = generator->height_offset;
    generator->_max_value = generator->height_offset;
    for (x = 0; x < generator->level_count; x++)
    {
        generator->_min_value += generator->levels[x].minvalue;
        generator->_max_value += generator->levels[x].minvalue + generator->levels[x].amplitude;
    }
}

void noiseRandomizeOffsets(NoiseGenerator* generator)
{
    int i;
    for (i = 0; i < MAX_LEVEL_COUNT; i++)
    {
        generator->levels[i].xoffset = toolsRandom();
        generator->levels[i].yoffset = toolsRandom();
        generator->levels[i].zoffset = toolsRandom();
    }
}

NoiseFunction noiseGetFunction(NoiseGenerator* generator)
{
    return generator->function;
}

void noiseSetFunction(NoiseGenerator* generator, NoiseFunction* function)
{
    generator->function = *function;
    noiseValidate(generator);
}

void noiseSetCustomFunction(NoiseGenerator* generator, double (*func1d)(double x), double (*func2d)(double x, double y), double (*func3d)(double x, double y, double z))
{
    generator->_func_noise_1d = func1d;
    generator->_func_noise_2d = func2d;
    generator->_func_noise_3d = func3d;
}

void noiseSetFunctionParams(NoiseGenerator* generator, NoiseFunctionAlgorithm algorithm, double ridge_factor, double curve_factor)
{
    NoiseFunction function = {algorithm, ridge_factor, curve_factor};
    noiseSetFunction(generator, &function);
}

void noiseForceValue(NoiseGenerator* generator, double value)
{
    noiseClearLevels(generator);
    generator->height_offset = value;
    noiseAddLevelSimple(generator, 1.0, 0.0, 0.0); /* FIXME Should not be needed */
}

void noiseGetRange(NoiseGenerator* generator, double* minvalue, double* maxvalue)
{
    *minvalue = generator->_min_value;
    *maxvalue = generator->_max_value;
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

void noiseAddLevel(NoiseGenerator* generator, NoiseLevel level, int protect_offsets)
{
    if (generator->level_count < MAX_LEVEL_COUNT)
    {
        NoiseLevel baselevel = generator->levels[generator->level_count];

        if (protect_offsets)
        {
            level.xoffset = baselevel.xoffset;
            level.yoffset = baselevel.yoffset;
            level.zoffset = baselevel.zoffset;
        }

        generator->levels[generator->level_count] = level;
        generator->level_count++;
        noiseValidate(generator);
    }
}

void noiseAddLevelSimple(NoiseGenerator* generator, double scaling, double minvalue, double maxvalue)
{
    NoiseLevel level;

    level.wavelength = scaling;
    level.minvalue = minvalue;
    level.amplitude = maxvalue - minvalue;

    noiseAddLevel(generator, level, 1);
}

void noiseAddLevels(NoiseGenerator* generator, int level_count, NoiseLevel start_level, double scaling_factor, double amplitude_factor, double center_factor)
{
    int i;

    for (i = 0; i < level_count; i++)
    {
        noiseAddLevel(generator, start_level, 1);
        start_level.minvalue += start_level.amplitude * (1.0 - amplitude_factor) * center_factor;
        start_level.wavelength *= scaling_factor;
        start_level.amplitude *= amplitude_factor;
    }
}

void noiseAddLevelsSimple(NoiseGenerator* generator, int level_count, double scaling, double minvalue, double maxvalue, double center_factor)
{
    NoiseLevel level;

    level.wavelength = scaling;
    level.minvalue = minvalue;
    level.amplitude = maxvalue - minvalue;
    noiseAddLevels(generator, level_count, level, 0.5, 0.5, center_factor);
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

void noiseSetLevel(NoiseGenerator* generator, int index, NoiseLevel level, int protect_offsets)
{
    if (index >= 0 && index < generator->level_count)
    {
        NoiseLevel baselevel = generator->levels[index];

        if (protect_offsets)
        {
            level.xoffset = baselevel.xoffset;
            level.yoffset = baselevel.yoffset;
            level.zoffset = baselevel.zoffset;
        }

        generator->levels[index] = level;
        noiseValidate(generator);
    }
}

void noiseSetLevelSimple(NoiseGenerator* generator, int index, double scaling, double minvalue, double maxvalue)
{
    NoiseLevel level;

    level.wavelength = scaling;
    level.minvalue = minvalue;
    level.amplitude = maxvalue - minvalue;

    noiseSetLevel(generator, index, level, 1);
}

void noiseNormalizeAmplitude(NoiseGenerator* generator, double minvalue, double maxvalue, int adjust_scaling)
{
    int level;
    double current_minvalue, current_maxvalue, current_amplitude;
    double target_amplitude, factor;

    if (generator->level_count == 0)
    {
        return;
    }

    target_amplitude = maxvalue - minvalue;
    noiseGetRange(generator, &current_minvalue, &current_maxvalue);
    current_amplitude = current_maxvalue - current_minvalue;
    factor = target_amplitude / current_amplitude;

    for (level = 0; level < generator->level_count; level++)
    {
        generator->levels[level].minvalue *= factor;
        generator->levels[level].amplitude *= factor;
        if (adjust_scaling)
        {
            generator->levels[level].wavelength *= factor;
        }
    }
    generator->height_offset = minvalue + (generator->height_offset - current_minvalue) * factor;
    noiseValidate(generator);
}

static inline double _fixValue(double value, double ridge, double curve)
{
    if (value < 0.0)
    {
        value = 0.0;
    }
    else if (value > 1.0)
    {
        value = 1.0;
    }

    if (curve > 0.0)
    {
        value = value * (1.0 - curve) + sqrt(value) * curve;
    }
    else if (curve < 0.0)
    {
        value = value * (1.0 - curve) + value * value * curve;
    }

    if (ridge > 0.0)
    {
        return fabs(value - ridge) / (1.0 - ridge);
    }
    else if (ridge < 0.0)
    {
        return 1.0 - fabs(value - 1.0 - ridge) / (1.0 + ridge);
    }
    else
    {
        return value;
    }
    /*if (ridge > 0.0)
    {
        return fabs(value + 0.5 - ridge) - 0.5 + ridge;
    }
    else if (ridge < 0.0)
    {
        return -fabs(value - 0.5 - ridge) + 0.5 + ridge;
    }
    else
    {
        return value;
    }*/
}




static inline double _get1DLevelValue(NoiseGenerator* generator, NoiseLevel* level, double x)
{
    return level->minvalue + _fixValue(generator->_func_noise_1d(x / level->wavelength + level->xoffset), generator->function.ridge_factor, generator->function.curve_factor) * level->amplitude;
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
        height = generator->levels[level].amplitude;
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




static inline double _get2DLevelValue(NoiseGenerator* generator, NoiseLevel* level, double x, double y)
{
    return level->minvalue + _fixValue(generator->_func_noise_2d(x / level->wavelength + level->xoffset, y / level->wavelength + level->yoffset), generator->function.ridge_factor, generator->function.curve_factor) * level->amplitude;
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
        height = generator->levels[level].amplitude;
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




static inline double _get3DLevelValue(NoiseGenerator* generator, NoiseLevel* level, double x, double y, double z)
{
    return level->minvalue + _fixValue(generator->_func_noise_3d(x / level->wavelength + level->xoffset, y / level->wavelength + level->yoffset, z / level->wavelength + level->zoffset), generator->function.ridge_factor, generator->function.curve_factor) * level->amplitude;
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
        height = generator->levels[level].amplitude;
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
