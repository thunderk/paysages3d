#include "NoiseGenerator.h"

#include "NoiseFunctionPerlin.h"
#include "NoiseFunctionSimplex.h"
#include "PackStream.h"
#include "RandomGenerator.h"
#include "NoiseState.h"
#include <cmath>
#include <cstdlib>
#include <cstring>

#define MAX_LEVEL_COUNT 30

/* NoiseGenerator class */

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

        stream->write(&level->frequency);
        stream->write(&level->amplitude);
        stream->write(&level->minvalue);
    }

    state.save(stream);
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

    if (level_count > MAX_LEVEL_COUNT)
    {
        level_count = MAX_LEVEL_COUNT;
    }

    for (x = 0; x < level_count; x++)
    {
        NoiseLevel* level = levels + x;

        stream->read(&level->frequency);
        stream->read(&level->amplitude);
        stream->read(&level->minvalue);
    }

    state.load(stream);

    validate();
}

void NoiseGenerator::copy(NoiseGenerator* destination)
{
    destination->function = function;
    destination->height_offset = height_offset;
    destination->level_count = level_count;

    memcpy(destination->levels, levels, sizeof(NoiseLevel) * destination->level_count);

    state.copy(&destination->state);

    validate();
}

void NoiseGenerator::validate()
{
    int x;

    if (function.algorithm < 0 || function.algorithm > NOISE_FUNCTION_SIMPLEX)
    {
        function.algorithm = NOISE_FUNCTION_SIMPLEX;
    }
    switch (function.algorithm)
    {
    case NOISE_FUNCTION_PERLIN:
        _func_noise_1d = noisePerlinGet1DValue;
        _func_noise_2d = noisePerlinGet2DValue;
        _func_noise_3d = noisePerlinGet3DValue;
        break;
    case NOISE_FUNCTION_SIMPLEX:
        _func_noise_1d = noiseSimplexGet1DValue;
        _func_noise_2d = noiseSimplexGet2DValue;
        _func_noise_3d = noiseSimplexGet3DValue;
        break;
    case NOISE_FUNCTION_CUSTOM:
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

void NoiseGenerator::setState(const NoiseState &state)
{
    state.copy(&this->state);
}

void NoiseGenerator::randomizeOffsets()
{
    state.randomizeOffsets();
}

NoiseGenerator::NoiseFunction NoiseGenerator::getFunction()
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
    _func_noise_1d = func1d;
    _func_noise_2d = func2d;
    _func_noise_3d = func3d;
    function.algorithm = NOISE_FUNCTION_CUSTOM;
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

void NoiseGenerator::addLevel(NoiseLevel level)
{
    if (level_count < MAX_LEVEL_COUNT)
    {
        levels[level_count] = level;
        level_count++;
        validate();
    }
}

void NoiseGenerator::addLevelSimple(double scaling, double minvalue, double maxvalue)
{
    NoiseLevel level;

    level.frequency = 1.0 / scaling;
    level.minvalue = minvalue;
    level.amplitude = maxvalue - minvalue;

    addLevel(level);
}

void NoiseGenerator::addLevels(int level_count, NoiseLevel start_level, double scaling_factor, double amplitude_factor, double center_factor)
{
    int i;

    for (i = 0; i < level_count; i++)
    {
        addLevel(start_level);
        start_level.minvalue += start_level.amplitude * (1.0 - amplitude_factor) * center_factor;
        start_level.frequency /= scaling_factor;
        start_level.amplitude *= amplitude_factor;
    }
}

void NoiseGenerator::addLevelsSimple(int level_count, double scaling, double minvalue, double maxvalue, double center_factor)
{
    NoiseLevel level;

    level.frequency = 1.0 / scaling;
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

void NoiseGenerator::setLevel(int index, NoiseLevel level)
{
    if (index >= 0 && index < level_count)
    {
        levels[index] = level;
        validate();
    }
}

void NoiseGenerator::setLevelSimple(int index, double scaling, double minvalue, double maxvalue)
{
    NoiseLevel level;

    level.frequency = 1.0 / scaling;
    level.minvalue = minvalue;
    level.amplitude = maxvalue - minvalue;

    setLevel(index, level);
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
            levels[level].frequency /= factor;
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




inline double NoiseGenerator::_get1DLevelValue(NoiseLevel* level, const NoiseState::NoiseOffset &offset, double x)
{
    return level->minvalue + _fixValue(_func_noise_1d(x * level->frequency + offset.x), function.ridge_factor, function.curve_factor) * level->amplitude;
}

double NoiseGenerator::get1DLevel(int level, double x)
{
    if (level >= 0 && level < level_count)
    {
        return _get1DLevelValue(levels + level, state.level_offsets[level], x);
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
        result += _get1DLevelValue(levels + level, state.level_offsets[level], x);
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

        result += _get1DLevelValue(levels + level, state.level_offsets[level], x) * factor;
    }
    return result + height_offset;
}




inline double NoiseGenerator::_get2DLevelValue(NoiseLevel* level, const NoiseState::NoiseOffset &offset, double x, double y)
{
    return level->minvalue + _fixValue(_func_noise_2d(x * level->frequency + offset.x, y * level->frequency + offset.y), function.ridge_factor, function.curve_factor) * level->amplitude;
}

double NoiseGenerator::get2DLevel(int level, double x, double y)
{
    if (level >= 0 && level < level_count)
    {
        return _get2DLevelValue(levels + level, state.level_offsets[level], x, y);
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
        result += _get2DLevelValue(levels + level, state.level_offsets[level], x, y);
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

        result += _get2DLevelValue(levels + level, state.level_offsets[level], x, y) * factor;
    }
    return result + height_offset;
}




inline double NoiseGenerator::_get3DLevelValue(NoiseLevel* level, const NoiseState::NoiseOffset &offset, double x, double y, double z)
{
    return level->minvalue + _fixValue(_func_noise_3d(x * level->frequency + offset.x, y * level->frequency + offset.y, z * level->frequency + offset.z), function.ridge_factor, function.curve_factor) * level->amplitude;
}

double NoiseGenerator::get3DLevel(int level, double x, double y, double z)
{
    if (level >= 0 && level < level_count)
    {
        return _get3DLevelValue(levels + level, state.level_offsets[level], x, y, z);
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
        result += _get3DLevelValue(levels + level, state.level_offsets[level], x, y, z);
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

        result += _get3DLevelValue(levels + level, state.level_offsets[level], x, y, z) * factor;
    }
    return result + height_offset;
}
