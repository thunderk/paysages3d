#ifndef NOISEGENERATOR_H
#define NOISEGENERATOR_H

#include "basics_global.h"

#define MAX_LEVEL_COUNT 30
#include "NoiseState.h"

namespace paysages {
namespace basics {

class BASICSSHARED_EXPORT NoiseGenerator
{
public:
    typedef enum
    {
        NOISE_FUNCTION_PERLIN,
        NOISE_FUNCTION_SIMPLEX,
        NOISE_FUNCTION_CUSTOM
    } NoiseFunctionAlgorithm;

    typedef struct
    {
        NoiseFunctionAlgorithm algorithm;
        double ridge_factor; /* -0.5;0.5 */
        double curve_factor; /* -1.0;1.0 */
    } NoiseFunction;

    typedef struct
    {
        double frequency;
        double amplitude;
        double minvalue;
    } NoiseLevel;

public:
    NoiseGenerator();
    virtual ~NoiseGenerator();

    virtual void save(PackStream* stream);
    virtual void load(PackStream* stream);
    virtual void copy(NoiseGenerator* destination);
    virtual void validate();

    inline const NoiseState &getState() const {return state;}
    void setState(const NoiseState &state);

    void randomizeOffsets();
    NoiseFunction getFunction();
    void setCustomFunction(double (*func1d)(double x), double (*func2d)(double x, double y), double (*func3d)(double x, double y, double z));
    void setFunction(NoiseFunction* function);
    void setFunctionParams(NoiseFunctionAlgorithm algorithm, double ridge_factor, double curve_factor);
    void forceValue(double value);
    void getRange(double* minvalue, double* maxvalue);
    int getLevelCount();
    void clearLevels();
    void addLevel(NoiseLevel level);
    void addLevelSimple(double scaling, double minvalue, double maxvalue);
    void addLevels(int level_count, NoiseLevel start_level, double scaling_factor, double amplitude_factor, double center_factor);
    void addLevelsSimple(int level_count, double scaling, double minvalue, double maxvalue, double center_factor);
    void removeLevel(int level);
    int getLevel(int level, NoiseLevel* params);
    void setLevel(int index, NoiseLevel level);
    void setLevelSimple(int index, double scaling, double minvalue, double maxvalue);
    void normalizeAmplitude(double minvalue, double maxvalue, int adjust_scaling);
    double get1DLevel(int level, double x);
    double get1DTotal(double x);
    double get1DDetail(double x, double detail);
    double get2DLevel(int level, double x, double y);
    double get2DTotal(double x, double y);
    double get2DDetail(double x, double y, double detail);
    double get3DLevel(int level, double x, double y, double z);
    double get3DTotal(double x, double y, double z);
    double get3DDetail(double x, double y, double z, double detail);

private:
    double _get1DLevelValue(NoiseLevel* level, const NoiseState::NoiseOffset &offset, double x);
    double _get2DLevelValue(NoiseLevel* level, const NoiseState::NoiseOffset &offset, double x, double y);
    double _get3DLevelValue(NoiseLevel* level, const NoiseState::NoiseOffset &offset, double x, double y, double z);

    NoiseFunction function;
    double height_offset;
    int level_count;
    NoiseLevel levels[MAX_LEVEL_COUNT];

    NoiseState state;

    double _min_value;
    double _max_value;
    double (*_func_noise_1d)(double x);
    double (*_func_noise_2d)(double x, double y);
    double (*_func_noise_3d)(double x, double y, double z);
};

}
}

#endif // NOISEGENERATOR_H
