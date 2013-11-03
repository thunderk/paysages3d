#include "NoiseGenerator.h"

#include "NoiseFunctionNaive.h"
#include "NoiseFunctionPerlin.h"
#include "NoiseFunctionSimplex.h"
#include "PackStream.h"
#include "RandomGenerator.h"
#include <cmath>

#define MAX_LEVEL_COUNT 30

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

NoiseGenerator::NoiseGenerator()
{
    function.algorithm = NOISE_FUNCTION_SIMPLEX;
    function.ridge_factor = 0.0;
    function.curve_factor = 0.0;
    level_count = 0;
    height_offset = 0.0;

    randomizeOffsets();
    validate();
}

NoiseGenerator::~NoiseGenerator()
{
}

void NoiseGenerator::save(PackStream* stream)
{
    int x;

    x = (int)function.algorithm;
    stream->write(&x);
    stream->write(&function.ridge_factor);
    stream->write(&function.curve_factor);

    stream->write(&height_offset);
    stream->write(&level_count);

    for (x = 0; x < level_count; x++)
    {
        NoiseLevel* level = levels + x;

        stream->write(&level->wavelength);
        stream->write(&level->amplitude);
        stream->write(&level->minvalue);
        stream->write(&level->xoffset);
        stream->write(&level->yoffset);
        stream->write(&level->zoffset);
    }
}

void NoiseGenerator::load(PackStream* stream)
{
    int x;

    stream->read(&x);
    function.algorithm = (NoiseFunctionAlgorithm)x;
    stream->read(&function.ridge_factor);
    stream->read(&function.curve_factor);

    stream->read(&height_offset);
    stream->read(&level_count);

    for (x = 0; x < level_count; x++)
    {
        NoiseLevel* level = levels + x;

        stream->read(&level->wavelength);
        stream->read(&level->amplitude);
        stream->read(&level->minvalue);
        stream->read(&level->xoffset);
        stream->read(&level->yoffset);
        stream->read(&level->zoffset);
    }

    validate();
}

void NoiseGenerator::copy(NoiseGenerator* destination)
{
    destination->function = function;
    destination->height_offset = height_offset;
    destination->level_count = level_count;

    memcpy(destination->levels, levels, sizeof(NoiseLevel) * destination->level_count);

    validate();
}

void NoiseGenerator::validate()
{
    int x;

    if (function.algorithm < 0 || function.algorithm > NOISE_FUNCTION_NAIVE)
    {
        function.algorithm = NOISE_FUNCTION_SIMPLEX;
    }
    switch (function.algorithm)
    {
    case NOISE_FUNCTION_SIMPLEX:
        _func_noise_1d = noiseSimplexGet1DValue;
        _func_noise_2d = noiseSimplexGet2DValue;
        _func_noise_3d = noiseSimplexGet3DValue;
        break;
    case NOISE_FUNCTION_PERLIN:
        _func_noise_1d = noisePerlinGet1DValue;
        _func_noise_2d = noisePerlinGet2DValue;
        _func_noise_3d = noisePerlinGet3DValue;
        break;
    case NOISE_FUNCTION_NAIVE:
        _func_noise_1d = noiseNaiveGet1DValue;
        _func_noise_2d = noiseNaiveGet2DValue;
        _func_noise_3d = noiseNaiveGet3DValue;
        break;
    }

    if (function.ridge_factor > 0.5)
    {
        function.ridge_factor = 0.5;
    }
    if (function.ridge_factor < -0.5)
    {
        function.ridge_factor = -0.5;
    }
    if (function.curve_factor > 1.0)
    {
        function.curve_factor = 1.0;
    }
    if (function.curve_factor < -1.0)
    {
        function.curve_factor = -1.0;
    }

    _min_value = height_offset;
    _max_value = height_offset;
    for (x = 0; x < level_count; x++)
    {
        _min_value += levels[x].minvalue;
        _max_value += levels[x].minvalue + levels[x].amplitude;
    }
}

void NoiseGenerator::randomizeOffsets()
{
    int i;
    for (i = 0; i < MAX_LEVEL_COUNT; i++)
    {
        levels[i].xoffset = RandomGenerator::random();
        levels[i].yoffset = RandomGenerator::random();
        levels[i].zoffset = RandomGenerator::random();
    }
}

NoiseFunction NoiseGenerator::getFunction()
{
    return function;
}

void NoiseGenerator::setFunction(NoiseFunction* function)
{
    this->function = *function;
    validate();
}

void NoiseGenerator::setCustomFunction(double (*func1d)(double x), double (*func2d)(double x, double y), double (*func3d)(double x, double y, double z))
{
    this->_func_noise_1d = func1d;
    this->_func_noise_2d = func2d;
    this->_func_noise_3d = func3d;
}

void NoiseGenerator::setFunctionParams(NoiseFunctionAlgorithm algorithm, double ridge_factor, double curve_factor)
{
    NoiseFunction function = {algorithm, ridge_factor, curve_factor};
    setFunction(&function);
}

void NoiseGenerator::forceValue(double value)
{
    clearLevels();
    height_offset = value;
    addLevelSimple(1.0, 0.0, 0.0); /* FIXME Should not be needed */
}

void NoiseGenerator::getRange(double* minvalue, double* maxvalue)
{
    *minvalue = _min_value;
    *maxvalue = _max_value;
}

int NoiseGenerator::getLevelCount()
{
    return level_count;
}

void NoiseGenerator::clearLevels()
{
    level_count = 0;
    validate();
}

void NoiseGenerator::addLevel(NoiseLevel level, int protect_offsets)
{
    if (level_count < MAX_LEVEL_COUNT)
    {
        NoiseLevel baselevel = levels[level_count];

        if (protect_offsets)
        {
            level.xoffset = baselevel.xoffset;
            level.yoffset = baselevel.yoffset;
            level.zoffset = baselevel.zoffset;
        }

        levels[level_count] = level;
        level_count++;
        validate();
    }
}

void NoiseGenerator::addLevelSimple(double scaling, double minvalue, double maxvalue)
{
    NoiseLevel level;

    level.wavelength = scaling;
    level.minvalue = minvalue;
    level.amplitude = maxvalue - minvalue;

    addLevel(level, 1);
}

void NoiseGenerator::addLevels(int level_count, NoiseLevel start_level, double scaling_factor, double amplitude_factor, double center_factor)
{
    int i;

    for (i = 0; i < level_count; i++)
    {
        addLevel(start_level, 1);
        start_level.minvalue += start_level.amplitude * (1.0 - amplitude_factor) * center_factor;
        start_level.wavelength *= scaling_factor;
        start_level.amplitude *= amplitude_factor;
    }
}

void NoiseGenerator::addLevelsSimple(int level_count, double scaling, double minvalue, double maxvalue, double center_factor)
{
    NoiseLevel level;

    level.wavelength = scaling;
    level.minvalue = minvalue;
    level.amplitude = maxvalue - minvalue;
    addLevels(level_count, level, 0.5, 0.5, center_factor);
}

void NoiseGenerator::removeLevel(int level)
{
    if (level >= 0 && level < level_count)
    {
        if (level_count > 1 && level < level_count - 1)
        {
            memmove(levels + level, levels + level + 1, sizeof(NoiseLevel) * (level_count - level - 1));
        }
        level_count--;
        validate();
    }
}

int NoiseGenerator::getLevel(int level, NoiseLevel* params)
{
    if (level >= 0 && level < level_count)
    {
        *params = levels[level];
        return 1;
    }
    else
    {
        return 0;
    }
}

void NoiseGenerator::setLevel(int index, NoiseLevel level, int protect_offsets)
{
    if (index >= 0 && index < level_count)
    {
        NoiseLevel baselevel = levels[index];

        if (protect_offsets)
        {
            level.xoffset = baselevel.xoffset;
            level.yoffset = baselevel.yoffset;
            level.zoffset = baselevel.zoffset;
        }

        levels[index] = level;
        validate();
    }
}

void NoiseGenerator::setLevelSimple(int index, double scaling, double minvalue, double maxvalue)
{
    NoiseLevel level;

    level.wavelength = scaling;
    level.minvalue = minvalue;
    level.amplitude = maxvalue - minvalue;

    setLevel(index, level, 1);
}

void NoiseGenerator::normalizeAmplitude(double minvalue, double maxvalue, int adjust_scaling)
{
    int level;
    double current_minvalue, current_maxvalue, current_amplitude;
    double target_amplitude, factor;

    if (level_count == 0)
    {
        return;
    }

    target_amplitude = maxvalue - minvalue;
    getRange(&current_minvalue, &current_maxvalue);
    current_amplitude = current_maxvalue - current_minvalue;
    factor = target_amplitude / current_amplitude;

    for (level = 0; level < level_count; level++)
    {
        levels[level].minvalue *= factor;
        levels[level].amplitude *= factor;
        if (adjust_scaling)
        {
            levels[level].wavelength *= factor;
        }
    }
    height_offset = minvalue + (height_offset - current_minvalue) * factor;
    validate();
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




inline double NoiseGenerator::_get1DLevelValue(NoiseLevel* level, double x)
{
    return level->minvalue + _fixValue(_func_noise_1d(x / level->wavelength + level->xoffset), function.ridge_factor, function.curve_factor) * level->amplitude;
}

double NoiseGenerator::get1DLevel(int level, double x)
{
    if (level >= 0 && level < level_count)
    {
        return _get1DLevelValue(levels + level, x);
    }
    else
    {
        return 0.0;
    }
}

double NoiseGenerator::get1DTotal(double x)
{
    int level;
    double result;

    result = 0.0;
    for (level = 0; level < level_count; level++)
    {
        result += _get1DLevelValue(levels + level, x);
    }
    return result + height_offset;
}

double NoiseGenerator::get1DDetail(double x, double detail)
{
    int level;
    double result, height, factor;

    result = 0.0;
    for (level = 0; level < level_count; level++)
    {
        height = levels[level].amplitude;
        factor = 1.0;
        if (height < detail * 0.25)
        {
            break;
        }
        else if (height < detail * 0.5)
        {
            factor = (detail * 0.5 - height) / 0.25;
        }

        result += _get1DLevelValue(levels + level, x) * factor;
    }
    return result + height_offset;
}




inline double NoiseGenerator::_get2DLevelValue(NoiseLevel* level, double x, double y)
{
    return level->minvalue + _fixValue(_func_noise_2d(x / level->wavelength + level->xoffset, y / level->wavelength + level->yoffset), function.ridge_factor, function.curve_factor) * level->amplitude;
}

double NoiseGenerator::get2DLevel(int level, double x, double y)
{
    if (level >= 0 && level < level_count)
    {
        return _get2DLevelValue(levels + level, x, y);
    }
    else
    {
        return 0.0;
    }
}

double NoiseGenerator::get2DTotal(double x, double y)
{
    int level;
    double result;

    result = 0.0;
    for (level = 0; level < level_count; level++)
    {
        result += _get2DLevelValue(levels + level, x, y);
    }
    return result + height_offset;
}

double NoiseGenerator::get2DDetail(double x, double y, double detail)
{
    int level;
    double result, height, factor;

    result = 0.0;
    for (level = 0; level < level_count; level++)
    {
        height = levels[level].amplitude;
        factor = 1.0;
        if (height < detail * 0.25)
        {
            break;
        }
        else if (height < detail * 0.5)
        {
            factor = (detail * 0.5 - height) / 0.25;
        }

        result += _get2DLevelValue(levels + level, x, y) * factor;
    }
    return result + height_offset;
}




inline double NoiseGenerator::_get3DLevelValue(NoiseLevel* level, double x, double y, double z)
{
    return level->minvalue + _fixValue(_func_noise_3d(x / level->wavelength + level->xoffset, y / level->wavelength + level->yoffset, z / level->wavelength + level->zoffset), function.ridge_factor, function.curve_factor) * level->amplitude;
}

double NoiseGenerator::get3DLevel(int level, double x, double y, double z)
{
    if (level >= 0 && level < level_count)
    {
        return _get3DLevelValue(levels + level, x, y, z);
    }
    else
    {
        return 0.0;
    }
}

double NoiseGenerator::get3DTotal(double x, double y, double z)
{
    int level;
    double result;

    result = 0.0;
    for (level = 0; level < level_count; level++)
    {
        result += _get3DLevelValue(levels + level, x, y, z);
    }
    return result + height_offset;
}

double NoiseGenerator::get3DDetail(double x, double y, double z, double detail)
{
    int level;
    double result, height, factor;

    result = 0.0;
    for (level = 0; level < level_count; level++)
    {
        height = levels[level].amplitude;
        factor = 1.0;
        if (height < detail * 0.25)
        {
            break;
        }
        else if (height < detail * 0.5)
        {
            factor = (detail * 0.5 - height) / 0.25;
        }

        result += _get3DLevelValue(levels + level, x, y, z) * factor;
    }
    return result + height_offset;
}
